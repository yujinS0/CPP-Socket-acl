using System.Text;

namespace WinFormsClient;

internal class PacketDefinition
{
    public enum PacketID : ushort
    {
        ReqLogin = 1002,
        ResLogin = 1003,
        ReqRoomEnter = 1021,
        ResRoomEnter = 1022,
        ReqRoomChat = 1026,
        ResRoomChat = 1027,
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

    public class RoomEnterRequest : PacketHeader
    {
        public int RoomNumber { get; set; }

        public byte[] Serialize()
        {
            byte[] headerBuffer = base.Serialize();
            byte[] roomNumberBuffer = BitConverter.GetBytes(RoomNumber);

            byte[] buffer = new byte[headerBuffer.Length + roomNumberBuffer.Length];
            Array.Copy(headerBuffer, 0, buffer, 0, headerBuffer.Length);
            Array.Copy(roomNumberBuffer, 0, buffer, headerBuffer.Length, roomNumberBuffer.Length);

            return buffer;
        }

        public static RoomEnterRequest Deserialize(byte[] buffer)
        {
            return new RoomEnterRequest
            {
                TotalSize = BitConverter.ToUInt16(buffer, 0),
                Id = (PacketID)BitConverter.ToUInt16(buffer, 2),
                Type = buffer[4],
                RoomNumber = BitConverter.ToInt32(buffer, 6)
            };
        }
    }

    public class RoomChatRequest : PacketHeader
    {
        public string Message { get; set; }

        public byte[] Serialize()
        {
            byte[] headerBuffer = base.Serialize();
            byte[] messageBuffer = Encoding.UTF8.GetBytes(Message);

            byte[] buffer = new byte[headerBuffer.Length + messageBuffer.Length];
            Array.Copy(headerBuffer, 0, buffer, 0, headerBuffer.Length);
            Array.Copy(messageBuffer, 0, buffer, headerBuffer.Length, messageBuffer.Length);

            return buffer;
        }

        public static RoomChatRequest Deserialize(byte[] buffer)
        {
            return new RoomChatRequest
            {
                TotalSize = BitConverter.ToUInt16(buffer, 0),
                Id = (PacketID)BitConverter.ToUInt16(buffer, 2),
                Type = buffer[4],
                Message = Encoding.UTF8.GetString(buffer, 6, buffer.Length - 6)
            };
        }
    }
}
