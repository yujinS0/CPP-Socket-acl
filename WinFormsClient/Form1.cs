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
            AddLogMessage("���α׷� ����");
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
            AddLogMessage("�α��� ��û ����");

            byte[] responseBuffer = new byte[256];
            int bytesRead = _stream.Read(responseBuffer, 0, responseBuffer.Length);

            string responseMessage = Encoding.UTF8.GetString(responseBuffer, 0, bytesRead);
            AddLogMessage($"���� ����: {responseMessage}");
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
            AddLogMessage($"�� ���� ��û ����: Room {roomNumber}");
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
            AddLogMessage($"ä�� �޽��� ����: {message}");
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
                        AddLogMessage($"[�α�] �����κ��� �����͸� �޴� �� ���� �߻�: {ex.Message}");
                        MessageBox.Show($"Failed to receive data from server: {ex.Message}");
                    }));
                }
            });
        }

        private void ProcessReceivedData(byte[] buffer)
        {
            PacketHeader header = PacketHeader.Deserialize(buffer);

            // ��Ŷ ID�� ���� �α� ���
            AddLogMessage($"���ŵ� ��Ŷ ID: {header.Id}");

            switch (header.Id)
            {
                case PacketID.NtfRoomChat:
                    var chatNotification = RoomChatNotification.Deserialize(buffer);

                    // ä�� �޽��� �� ����� ID ����� �α� �߰�
                    AddLogMessage($"���ŵ� �޽���: {chatNotification.UserID}, ����: {chatNotification.Message}");

                    // �� ���� ���� ó��
                    var cleanMessage = chatNotification.Message.TrimEnd('\0');
                    var cleanUserID = chatNotification.UserID.TrimEnd('\0');

                    // ä�� �޽����� ����Ʈ�� �߰��ϴ� �Լ� ȣ��
                    Invoke((Action)(() =>
                    {
                        try
                        {
                            AddRoomChatMessageList(cleanUserID, cleanMessage);
                        }
                        catch (Exception ex)
                        {
                            AddLogMessage($"UI ������Ʈ �� ���� �߻�: {ex.Message}");
                        }
                    }));
                    break;

                case PacketID.NtfUserList:
                    var userListNotification = UserListNotification.Deserialize(buffer);

                    // ���� ��� ���� �α� �߰�
                    AddLogMessage($"���� ��� ����: {userListNotification.UserID1}, {userListNotification.UserID2}");

                    // �� ���� ���� ó��
                    var cleanUserID1 = userListNotification.UserID1.TrimEnd('\0');
                    var cleanUserID2 = userListNotification.UserID2.TrimEnd('\0');

                    Invoke((Action)(() =>
                    {
                        try
                        {
                            AddLogMessage("[�α�] ���� ����� �����߽��ϴ�.");
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
                            AddLogMessage($"UI ������Ʈ �� ���� �߻�: {ex.Message}");
                        }
                    }));
                    break;

                default:
                    Invoke((Action)(() =>
                    {
                        AddLogMessage($"�� �� ���� ��Ŷ ����: {header.Id}");
                    }));
                    break;
            }
        }

        // ä�� �޽����� ����Ʈ�� �߰��ϴ� �Լ�
        private void AddRoomChatMessageList(string userID, string message)
        {
            // '\0' ���ڰ� ������ �� �������� ���ڿ��� ���
            int nullCharIndexUserID = userID.IndexOf('\0');
            if (nullCharIndexUserID >= 0)
            {
                userID = userID.Substring(0, nullCharIndexUserID);
            }

            int nullCharIndexMessage = message.IndexOf('\0');
            if (nullCharIndexMessage >= 0)
            {
                message = message.Substring(0, nullCharIndexMessage);
            }

            // �ִ� �޽��� ���� 512�� ����
            if (lstChatMessages.Items.Count > 512)
            {
                lstChatMessages.Items.Clear();
            }

            // ���ο� �޽����� �߰��ϰ�, ����Ʈ�ڽ��� �� �Ʒ��� ��ũ��
            lstChatMessages.Items.Add($"[{userID}]: {message}");
            lstChatMessages.SelectedIndex = lstChatMessages.Items.Count - 1;
        }


        public void AddLogMessage(string message)
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
