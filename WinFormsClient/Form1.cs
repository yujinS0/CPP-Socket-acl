using static WinFormsClient.PacketDefinition;
using System.Net.Sockets;
using System.Text;

namespace WinFormsClient;

public partial class Form1 : Form
{
    private TcpClient _client;
    private NetworkStream _stream;
    public Form1()
    {
        InitializeComponent();
    }

    private void btnConnect_Click(object sender, EventArgs e)
    {
        string serverAddress = txtServerAddress.Text;

        try
        {
            _client = new TcpClient(serverAddress, 8088);
            _stream = _client.GetStream();
            MessageBox.Show("Connected to server!");
        }
        catch (Exception ex)
        {
            MessageBox.Show($"Failed to connect to server: {ex.Message}");
        }
    }

    private void btnLogin_Click(object sender, EventArgs e)
    {
        string userID = txtUserID.Text;
        string authToken = txtPassword.Text;

        if (_stream == null)
        {
            MessageBox.Show("Not connected to the server.");
            return;
        }

        // ��Ŷ ����ȭ
        var loginRequest = new LoginRequest
        {
            TotalSize = 70, // ��Ŷ ��ü ũ��
            Id = PacketID.ReqLogin,
            Type = 0, // �Ϲ� ��Ŷ Ÿ��
            UserID = userID,
            AuthToken = authToken
        };

        byte[] packetData = loginRequest.Serialize();

        // ������ ��Ŷ ����
        _stream.Write(packetData, 0, packetData.Length);

        // �����κ��� ���� ����
        byte[] responseBuffer = new byte[256];
        int bytesRead = _stream.Read(responseBuffer, 0, responseBuffer.Length);

        string responseMessage = Encoding.UTF8.GetString(responseBuffer, 0, bytesRead);
        MessageBox.Show(responseMessage);
    }

    private void btnEnterRoom_Click(object sender, EventArgs e)
    {
        int roomNumber = int.Parse(txtRoomNumber.Text);

        if (_stream == null)
        {
            MessageBox.Show("Not connected to the server.");
            return;
        }

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
    }

    private void btnSendChat_Click(object sender, EventArgs e)
    {
        string message = txtChatMessage.Text;

        if (_stream == null)
        {
            MessageBox.Show("Not connected to the server.");
            return;
        }

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
                    byte[] buffer = new byte[256];
                    int bytesRead = _stream.Read(buffer, 0, buffer.Length);
                    if (bytesRead > 0)
                    {
                        Invoke((Action)(() =>
                        {
                            string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);
                            txtChatDisplay.AppendText($"{message}{Environment.NewLine}");
                        }));
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"Failed to receive data from server: {ex.Message}");
                    break;
                }
            }
        });
    }

    private void LoginForm_FormClosing(object sender, FormClosingEventArgs e)
    {
        _stream?.Close();
        _client?.Close();
    }
}
