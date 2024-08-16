using System;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static WinFormsClient.PacketDefinition;

namespace WinFormsClient
{
    public partial class Form1 : Form
    {
        private TcpClient _client;
        private NetworkStream _stream;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            AddLogMessage("프로그램 시작");
        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            string serverAddress = txtServerAddress.Text;

            try
            {
                _client = new TcpClient(serverAddress, 8088);
                _stream = _client.GetStream();
                MessageBox.Show("Connected to server!");
                AddLogMessage("Connected to server!");
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Failed to connect to server: {ex.Message}");
                AddLogMessage($"Failed to connect to server: {ex.Message}");
            }
        }

        private void btnLogin_Click(object sender, EventArgs e)
        {
            if (_stream == null)
            {
                MessageBox.Show("Not connected to the server.");
                return;
            }

            string userID = txtUserID.Text;
            string authToken = txtPassword.Text;

            var loginRequest = new LoginRequest
            {
                TotalSize = 70,
                Id = PacketID.ReqLogin,
                Type = 0,
                UserID = userID,
                AuthToken = authToken
            };

            byte[] packetData = loginRequest.Serialize();
            _stream.Write(packetData, 0, packetData.Length);
            AddLogMessage("로그인 요청 전송");

            byte[] responseBuffer = new byte[256];
            int bytesRead = _stream.Read(responseBuffer, 0, responseBuffer.Length);

            string responseMessage = Encoding.UTF8.GetString(responseBuffer, 0, bytesRead);
            AddLogMessage($"서버 응답: {responseMessage}");
            MessageBox.Show(responseMessage);
        }

        private void btnEnterRoom_Click(object sender, EventArgs e)
        {
            StartReceiving();

            if (_stream == null)
            {
                MessageBox.Show("Not connected to the server.");
                return;
            }

            int roomNumber = int.Parse(txtRoomNumber.Text);
            var roomEnterRequest = new RoomEnterRequest
            {
                TotalSize = 10,
                Id = PacketID.ReqRoomEnter,
                Type = 0,
                RoomNumber = roomNumber
            };

            byte[] packetData = roomEnterRequest.Serialize();
            _stream.Write(packetData, 0, packetData.Length);
            AddLogMessage($"방 입장 요청 전송: Room {roomNumber}");
        }

        private void btnSendChat_Click(object sender, EventArgs e)
        {
            if (_stream == null)
            {
                MessageBox.Show("Not connected to the server.");
                return;
            }

            string message = txtChatMessage.Text;
            var chatRequest = new RoomChatRequest
            {
                TotalSize = (ushort)(6 + message.Length),
                Id = PacketID.ReqRoomChat,
                Type = 0,
                Message = message
            };

            byte[] packetData = chatRequest.Serialize();
            _stream.Write(packetData, 0, packetData.Length);
            AddLogMessage($"채팅 메시지 전송: {message}");
            txtChatMessage.Clear();
        }

        private void StartReceiving()
        {
            Task.Run(() =>
            {
                try
                {
                    while (_client != null && _client.Connected)
                    {
                        byte[] buffer = new byte[512];
                        int bytesRead = _stream.Read(buffer, 0, buffer.Length);
                        if (bytesRead > 0)
                        {
                            ProcessReceivedData(buffer);
                        }
                    }
                }
                catch (Exception ex)
                {
                    Invoke((Action)(() =>
                    {
                        AddLogMessage($"[로그] 서버로부터 데이터를 받는 중 오류 발생: {ex.Message}");
                        MessageBox.Show($"Failed to receive data from server: {ex.Message}");
                    }));
                }
            });
        }

        private void ProcessReceivedData(byte[] buffer)
        {
            PacketHeader header = PacketHeader.Deserialize(buffer);

            switch (header.Id)
            {
                case PacketID.NtfRoomChat:
                    var chatNotification = RoomChatNotification.Deserialize(buffer);

                    // 널 문자 제거 처리
                    var cleanMessage = chatNotification.Message.TrimEnd('\0');
                    var cleanUserID = chatNotification.UserID.TrimEnd('\0');

                    Invoke((Action)(() =>
                    {
                        try
                        {
                            AddLogMessage($"서버로부터 받은 메시지 = {cleanUserID} : {cleanMessage}");
                            lstChatMessages.Items.Add($"{cleanUserID} : {cleanMessage}");
                        }
                        catch (Exception ex)
                        {
                            AddLogMessage($"UI 업데이트 중 오류 발생: {ex.Message}");
                        }
                    }));
                    break;

                case PacketID.NtfUserList:
                    var userListNotification = UserListNotification.Deserialize(buffer);

                    // 널 문자 제거 처리
                    var cleanUserID1 = userListNotification.UserID1.TrimEnd('\0');
                    var cleanUserID2 = userListNotification.UserID2.TrimEnd('\0');

                    Invoke((Action)(() =>
                    {
                        try
                        {
                            AddLogMessage("[로그] 유저 목록을 수신했습니다.");
                            lstUsers.Items.Clear();
                            if (!string.IsNullOrEmpty(cleanUserID1))
                            {
                                lstUsers.Items.Add(cleanUserID1);
                            }
                            if (!string.IsNullOrEmpty(cleanUserID2))
                            {
                                lstUsers.Items.Add(cleanUserID2);
                            }
                        }
                        catch (Exception ex)
                        {
                            AddLogMessage($"UI 업데이트 중 오류 발생: {ex.Message}");
                        }
                    }));
                    break;

                default:
                    Invoke((Action)(() =>
                    {
                        AddLogMessage($"알 수 없는 패킷 수신: {header.Id}");
                    }));
                    break;
            }
        }

        private void AddLogMessage(string message)
        {
            if (InvokeRequired)
            {
                Invoke(new Action(() => lstLog.Items.Add(message)));
            }
            else
            {
                lstLog.Items.Add(message);
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            _stream?.Close();
            _client?.Close();
        }
    }
}
