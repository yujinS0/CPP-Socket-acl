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

        // Form Load 이벤트 핸들러 추가
        private void Form1_Load(object sender, EventArgs e)
        {
            lstLog.Items.Add("프로그램 시작");
            // 추가적인 초기화 로직이 필요하다면 여기에 작성
        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            string serverAddress = txtServerAddress.Text;

            try
            {
                _client = new TcpClient(serverAddress, 8088);
                _stream = _client.GetStream();
                MessageBox.Show("Connected to server!");
                lstLog.Items.Add("Connected to server!");

                
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Failed to connect to server: {ex.Message}");
                lstLog.Items.Add($"Failed to connect to server: {ex.Message}");
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

            // 패킷 직렬화
            var loginRequest = new LoginRequest
            {
                TotalSize = 70,
                Id = PacketID.ReqLogin,
                Type = 0,
                UserID = userID,
                AuthToken = authToken
            };

            byte[] packetData = loginRequest.Serialize();

            // 서버로 패킷 전송
            _stream.Write(packetData, 0, packetData.Length);
            lstLog.Items.Add("로그인 요청 전송");

            // 서버 응답 대기
            byte[] responseBuffer = new byte[256];
            int bytesRead = _stream.Read(responseBuffer, 0, responseBuffer.Length);

            string responseMessage = Encoding.UTF8.GetString(responseBuffer, 0, bytesRead);
            lstLog.Items.Add($"서버 응답: {responseMessage}");
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

            // 서버로 방 입장 요청 전송
            _stream.Write(packetData, 0, packetData.Length);
            lstLog.Items.Add($"방 입장 요청 전송: Room {roomNumber}");
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

            // 서버로 채팅 메시지 전송
            _stream.Write(packetData, 0, packetData.Length);
            lstLog.Items.Add($"채팅 메시지 전송: {message}");
            txtChatMessage.Clear();
        }

        //private void StartReceiving()
        //{
        //    Task.Run(() =>
        //    {
        //        byte[] headerBuffer = new byte[6]; // 헤더 크기만큼 먼저 읽음
        //        int headerOffset = 0;

        //        while (_client != null && _client.Connected)
        //        {
        //            try
        //            {
        //                // 1단계: 먼저 고정된 크기의 헤더를 읽는다.
        //                int bytesRead = _stream.Read(headerBuffer, headerOffset, headerBuffer.Length - headerOffset);
        //                if (bytesRead > 0)
        //                {
        //                    headerOffset += bytesRead;

        //                    // 헤더를 모두 읽었는지 확인
        //                    if (headerOffset == headerBuffer.Length)
        //                    {
        //                        // 2단계: 헤더에서 TotalSize를 읽고 나머지 패킷을 처리
        //                        PacketHeader header = PacketHeader.Deserialize(headerBuffer);

        //                        byte[] fullPacket = new byte[header.TotalSize];
        //                        Array.Copy(headerBuffer, fullPacket, headerBuffer.Length);

        //                        int remainingBytes = header.TotalSize - headerBuffer.Length;
        //                        int packetOffset = headerBuffer.Length;

        //                        // 남은 데이터 읽기
        //                        while (remainingBytes > 0)
        //                        {
        //                            bytesRead = _stream.Read(fullPacket, packetOffset, remainingBytes);
        //                            if (bytesRead > 0)
        //                            {
        //                                remainingBytes -= bytesRead;
        //                                packetOffset += bytesRead;
        //                            }
        //                        }

        //                        // 3단계: 전체 패킷을 처리
        //                        switch (header.Id)
        //                        {
        //                            case PacketID.NtfRoomChat:
        //                                RoomChatRequest chatRequest = RoomChatRequest.Deserialize(fullPacket);
        //                                // 메시지 처리
        //                                break;
        //                        }

        //                        // 다시 헤더 읽기로 돌아가기
        //                        headerOffset = 0;
        //                    }
        //                }
        //            }
        //            catch (Exception ex)
        //            {
        //                // 오류 처리
        //                break;
        //            }
        //        }
        //    });
        //}
        private void StartReceiving()
        {
            Task.Run(() =>
            {
                while (_client != null && _client.Connected)
                {
                    try
                    {
                        byte[] buffer = new byte[294];
                        int bytesRead = _stream.Read(buffer, 0, buffer.Length);
                        if (bytesRead > 0)
                        {
                            PacketHeader header = PacketHeader.Deserialize(buffer);
                            switch (header.Id)
                            {
                                case PacketID.NtfRoomChat:
                                    var chatNotification = RoomChatNotification.Deserialize(buffer);

                                    // UI 스레드에서 안전하게 접근하기 위해 Invoke 호출
                                    this.Invoke((Action)(() =>
                                    {
                                        lstLog.Items.Add($"서버로부터 받은 메시지: {chatNotification.UserID}: {chatNotification.Message}");
                                        lstChatMessages.Items.Add($"{chatNotification.UserID}: {chatNotification.Message}");

                                        // 유저 목록 업데이트
                                        if (chatNotification.Message.StartsWith("User list"))
                                        {
                                            lstLog.Items.Add("[로그] 유저 목록을 수신했습니다.");
                                            lstUsers.Items.Clear();
                                            var users = chatNotification.Message.Substring(15).Split(' ');
                                            foreach (var user in users)
                                            {
                                                if (!string.IsNullOrEmpty(user))
                                                {
                                                    lstUsers.Items.Add(user);
                                                }
                                            }
                                        }

                                        // 유저 입장/퇴장 처리
                                        if (chatNotification.Message.EndsWith("has entered the room."))
                                        {
                                            if (!lstUsers.Items.Contains(chatNotification.UserID))
                                            {
                                                lstLog.Items.Add($"{chatNotification.UserID}님이 방에 입장했습니다.");
                                                lstUsers.Items.Add(chatNotification.UserID);
                                            }
                                        }
                                        if (chatNotification.Message.EndsWith("has left the room."))
                                        {
                                            lstLog.Items.Add($"{chatNotification.UserID}님이 방을 떠났습니다.");
                                            lstUsers.Items.Remove(chatNotification.UserID);
                                        }
                                    }));
                                    break;
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        // UI 스레드에서 접근하도록 Invoke로 감싸기
                        this.Invoke((Action)(() =>
                        {
                            lstLog.Items.Add($"[로그] 서버로부터 데이터를 받는 중 오류 발생: {ex.Message}");
                            MessageBox.Show($"Failed to receive data from server: {ex.Message}");
                        }));
                        break;
                    }
                }
            });
        }



        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            _stream?.Close();
            _client?.Close();
        }
    }
}
