using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WinFormsClient;

internal class PacketDefinition
{
    public enum PacketID : ushort
    {
        ReqLogin = 1002,
        ResLogin = 1003,
    }

    public class PacketHeader
    {
        public ushort TotalSize { get; set; }
        public PacketID Id { get; set; }
        public byte Type { get; set; }

        public byte[] Serialize()
        {
            byte[] buffer = new byte[6];
            BitConverter.GetBytes(TotalSize).CopyTo(buffer, 0);
            BitConverter.GetBytes((ushort)Id).CopyTo(buffer, 2);
            buffer[4] = Type;
            return buffer;
        }

        public static PacketHeader Deserialize(byte[] buffer)
        {
            return new PacketHeader
            {
                TotalSize = BitConverter.ToUInt16(buffer, 0),
                Id = (PacketID)BitConverter.ToUInt16(buffer, 2),
                Type = buffer[4]
            };
        }
    }

    public class LoginRequest : PacketHeader
    {
        public string UserID { get; set; }
        public string AuthToken { get; set; }

        public byte[] Serialize()
        {
            byte[] headerBuffer = base.Serialize();
            byte[] userIdBuffer = Encoding.UTF8.GetBytes(UserID.PadRight(32, '\0'));
            byte[] authTokenBuffer = Encoding.UTF8.GetBytes(AuthToken.PadRight(32, '\0'));

            byte[] buffer = new byte[headerBuffer.Length + userIdBuffer.Length + authTokenBuffer.Length];
            Array.Copy(headerBuffer, 0, buffer, 0, headerBuffer.Length);
            Array.Copy(userIdBuffer, 0, buffer, headerBuffer.Length, userIdBuffer.Length);
            Array.Copy(authTokenBuffer, 0, buffer, headerBuffer.Length + userIdBuffer.Length, authTokenBuffer.Length);

            return buffer;
        }

        public static LoginRequest Deserialize(byte[] buffer)
        {
            return new LoginRequest
            {
                TotalSize = BitConverter.ToUInt16(buffer, 0),
                Id = (PacketID)BitConverter.ToUInt16(buffer, 2),
                Type = buffer[4],
                UserID = Encoding.UTF8.GetString(buffer, 6, 32).TrimEnd('\0'),
                AuthToken = Encoding.UTF8.GetString(buffer, 38, 32).TrimEnd('\0')
            };
        }
    }
}