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

        // Form Load �̺�Ʈ �ڵ鷯 �߰�
        private void Form1_Load(object sender, EventArgs e)
        {
            lstLog.Items.Add("���α׷� ����");
            // �߰����� �ʱ�ȭ ������ �ʿ��ϴٸ� ���⿡ �ۼ�
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

            // ��Ŷ ����ȭ
            var loginRequest = new LoginRequest
            {
                TotalSize = 70,
                Id = PacketID.ReqLogin,
                Type = 0,
                UserID = userID,
                AuthToken = authToken
            };

            byte[] packetData = loginRequest.Serialize();

            // ������ ��Ŷ ����
            _stream.Write(packetData, 0, packetData.Length);
            lstLog.Items.Add("�α��� ��û ����");

            // ���� ���� ���
            byte[] responseBuffer = new byte[256];
            int bytesRead = _stream.Read(responseBuffer, 0, responseBuffer.Length);

            string responseMessage = Encoding.UTF8.GetString(responseBuffer, 0, bytesRead);
            lstLog.Items.Add($"���� ����: {responseMessage}");
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

            // ������ �� ���� ��û ����
            _stream.Write(packetData, 0, packetData.Length);
            lstLog.Items.Add($"�� ���� ��û ����: Room {roomNumber}");
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

            // ������ ä�� �޽��� ����
            _stream.Write(packetData, 0, packetData.Length);
            lstLog.Items.Add($"ä�� �޽��� ����: {message}");
            txtChatMessage.Clear();
        }

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

                                    // UI �����忡�� �����ϰ� �����ϱ� ���� Invoke ȣ��
                                    this.Invoke((Action)(() =>
                                    {
                                        lstLog.Items.Add($"�����κ��� ���� �޽���: {chatNotification.UserID}: {chatNotification.Message}");
                                        lstChatMessages.Items.Add($"{chatNotification.UserID}: {chatNotification.Message}");

                                        // ���� ��� ������Ʈ
                                        if (chatNotification.Message.StartsWith("User list"))
                                        {
                                            lstLog.Items.Add("[�α�] ���� ����� �����߽��ϴ�.");
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

                                        // ���� ����/���� ó��
                                        if (chatNotification.Message.EndsWith("has entered the room."))
                                        {
                                            if (!lstUsers.Items.Contains(chatNotification.UserID))
                                            {
                                                lstLog.Items.Add($"{chatNotification.UserID}���� �濡 �����߽��ϴ�.");
                                                lstUsers.Items.Add(chatNotification.UserID);
                                            }
                                        }
                                        if (chatNotification.Message.EndsWith("has left the room."))
                                        {
                                            lstLog.Items.Add($"{chatNotification.UserID}���� ���� �������ϴ�.");
                                            lstUsers.Items.Remove(chatNotification.UserID);
                                        }
                                    }));
                                    break;
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        // UI �����忡�� �����ϵ��� Invoke�� ���α�
                        this.Invoke((Action)(() =>
                        {
                            lstLog.Items.Add($"[�α�] �����κ��� �����͸� �޴� �� ���� �߻�: {ex.Message}");
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
