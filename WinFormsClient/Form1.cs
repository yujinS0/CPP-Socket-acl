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

        // 패킷 직렬화
        var loginRequest = new LoginRequest
        {
            TotalSize = 70, // 패킷 전체 크기
            Id = PacketID.ReqLogin,
            Type = 0, // 일반 패킷 타입
            UserID = userID,
            AuthToken = authToken
        };

        byte[] packetData = loginRequest.Serialize();

        // 서버로 패킷 전송
        _stream.Write(packetData, 0, packetData.Length);

        // 서버로부터 응답 수신
        byte[] responseBuffer = new byte[256];
        int bytesRead = _stream.Read(responseBuffer, 0, responseBuffer.Length);

        string responseMessage = Encoding.UTF8.GetString(responseBuffer, 0, bytesRead);
        MessageBox.Show(responseMessage);
    }

    private void LoginForm_FormClosing(object sender, FormClosingEventArgs e)
    {
        _stream?.Close();
        _client?.Close();
    }
}
