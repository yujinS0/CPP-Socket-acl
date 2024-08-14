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
        SuspendLayout();
        // 
        // txtServerAddress
        // 
        txtServerAddress.Location = new Point(71, 40);
        txtServerAddress.Name = "txtServerAddress";
        txtServerAddress.Size = new Size(260, 31);
        txtServerAddress.TabIndex = 0;
        txtServerAddress.Text = "127.0.0.1";
        // 
        // btnConnect
        // 
        btnConnect.Location = new Point(337, 40);
        btnConnect.Name = "btnConnect";
        btnConnect.Size = new Size(131, 31);
        btnConnect.TabIndex = 1;
        btnConnect.Text = "Connect";
        btnConnect.UseVisualStyleBackColor = true;
        btnConnect.Click += btnConnect_Click;
        // 
        // txtUserID
        // 
        txtUserID.Location = new Point(71, 95);
        txtUserID.Name = "txtUserID";
        txtUserID.Size = new Size(260, 31);
        txtUserID.TabIndex = 2;
        // 
        // txtPassword
        // 
        txtPassword.Location = new Point(71, 141);
        txtPassword.Name = "txtPassword";
        txtPassword.Size = new Size(260, 31);
        txtPassword.TabIndex = 3;
        // 
        // btnLogin
        // 
        btnLogin.Location = new Point(337, 106);
        btnLogin.Name = "btnLogin";
        btnLogin.Size = new Size(131, 57);
        btnLogin.TabIndex = 4;
        btnLogin.Text = "Login";
        btnLogin.UseVisualStyleBackColor = true;
        btnLogin.Click += btnLogin_Click;
        // 
        // Form1
        // 
        ClientSize = new Size(487, 366);
        Controls.Add(btnLogin);
        Controls.Add(txtPassword);
        Controls.Add(txtUserID);
        Controls.Add(btnConnect);
        Controls.Add(txtServerAddress);
        Name = "Form1";
        Text = "Login Form";
        FormClosing += LoginForm_FormClosing;
        ResumeLayout(false);
        PerformLayout();
    }

    private System.Windows.Forms.TextBox txtServerAddress;
    private System.Windows.Forms.Button btnConnect;
    private System.Windows.Forms.TextBox txtUserID;
    private System.Windows.Forms.TextBox txtPassword;
    private System.Windows.Forms.Button btnLogin;
}
