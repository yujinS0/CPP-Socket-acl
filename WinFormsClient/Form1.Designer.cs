namespace WinFormsClient;

partial class Form1
{
    private System.ComponentModel.IContainer components = null;

    protected override void Dispose(bool disposing)
    {
        if (disposing && (components != null))
        {
            components.Dispose();
        }
        base.Dispose(disposing);
    }

    private void InitializeComponent()
    {
        txtServerAddress = new TextBox();
        btnConnect = new Button();
        txtUserID = new TextBox();
        txtPassword = new TextBox();
        btnLogin = new Button();
        txtRoomNumber = new TextBox();
        btnEnterRoom = new Button();
        txtChatMessage = new TextBox();
        btnSendChat = new Button();
        lblDescription = new Label();

        SuspendLayout();
        // 
        // txtServerAddress
        // 
        txtServerAddress.Location = new Point(106, 60);
        txtServerAddress.Name = "txtServerAddress";
        txtServerAddress.Size = new Size(260, 31);
        txtServerAddress.TabIndex = 0;
        txtServerAddress.Text = "127.0.0.1";
        // 
        // btnConnect
        // 
        btnConnect.Location = new Point(372, 60);
        btnConnect.Name = "btnConnect";
        btnConnect.Size = new Size(131, 31);
        btnConnect.TabIndex = 1;
        btnConnect.Text = "Connect";
        btnConnect.UseVisualStyleBackColor = true;
        btnConnect.Click += btnConnect_Click;
        // 
        // txtUserID
        // 
        txtUserID.Location = new Point(106, 177);
        txtUserID.Name = "txtUserID";
        txtUserID.Size = new Size(260, 31);
        txtUserID.TabIndex = 2;
        // 
        // txtPassword
        // 
        txtPassword.Location = new Point(106, 223);
        txtPassword.Name = "txtPassword";
        txtPassword.Size = new Size(260, 31);
        txtPassword.TabIndex = 3;
        // 
        // btnLogin
        // 
        btnLogin.Location = new Point(372, 188);
        btnLogin.Name = "btnLogin";
        btnLogin.Size = new Size(131, 57);
        btnLogin.TabIndex = 4;
        btnLogin.Text = "Login";
        btnLogin.UseVisualStyleBackColor = true;
        btnLogin.Click += btnLogin_Click;
        // 
        // txtRoomNumber
        // 
        txtRoomNumber.Location = new Point(227, 342);
        txtRoomNumber.Name = "txtRoomNumber";
        txtRoomNumber.Size = new Size(139, 31);
        txtRoomNumber.TabIndex = 5;
        txtRoomNumber.Text = "1";
        // 
        // btnEnterRoom
        // 
        btnEnterRoom.Location = new Point(372, 342);
        btnEnterRoom.Name = "btnEnterRoom";
        btnEnterRoom.Size = new Size(131, 31);
        btnEnterRoom.TabIndex = 6;
        btnEnterRoom.Text = "Enter Room";
        btnEnterRoom.UseVisualStyleBackColor = true;
        btnEnterRoom.Click += btnEnterRoom_Click;
        // 
        // txtChatMessage
        // 
        txtChatMessage.Location = new Point(106, 640);
        txtChatMessage.Name = "txtChatMessage";
        txtChatMessage.Size = new Size(260, 31);
        txtChatMessage.TabIndex = 7;
        // 
        // btnSendChat
        // 
        btnSendChat.Location = new Point(372, 640);
        btnSendChat.Name = "btnSendChat";
        btnSendChat.Size = new Size(131, 31);
        btnSendChat.TabIndex = 8;
        btnSendChat.Text = "Send Chat";
        btnSendChat.UseVisualStyleBackColor = true;
        btnSendChat.Click += btnSendChat_Click;
        // 
        // lblDescription
        // 
        lblDescription.AutoSize = true;
        lblDescription.Location = new Point(71, 10);
        lblDescription.Name = "lblDescription";
        lblDescription.Size = new Size(397, 20);
        lblDescription.TabIndex = 10;
        lblDescription.Text = "서버에 접속한 후, 로그인하고 방에 입장하세요.";

        // 
        // Form1
        // 
        ClientSize = new Size(1030, 749);
        Controls.Add(btnSendChat);
        Controls.Add(txtChatMessage);
        Controls.Add(btnEnterRoom);
        Controls.Add(txtRoomNumber);
        Controls.Add(btnLogin);
        Controls.Add(txtPassword);
        Controls.Add(txtUserID);
        Controls.Add(btnConnect);
        Controls.Add(txtServerAddress);
        Name = "Form1";
        Text = "Test Form";
        FormClosing += LoginForm_FormClosing;
        ResumeLayout(false);
        PerformLayout();
    }

    private System.Windows.Forms.TextBox txtServerAddress;
    private System.Windows.Forms.Button btnConnect;
    private System.Windows.Forms.TextBox txtUserID;
    private System.Windows.Forms.TextBox txtPassword;
    private System.Windows.Forms.Button btnLogin;
    private System.Windows.Forms.TextBox txtRoomNumber;
    private System.Windows.Forms.Button btnEnterRoom; 
    private System.Windows.Forms.TextBox txtChatMessage; 
    private System.Windows.Forms.Button btnSendChat;
    private System.Windows.Forms.TextBox txtChatDisplay;
    private System.Windows.Forms.Label lblDescription;
}
