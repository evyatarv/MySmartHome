using Newtonsoft.Json;
using System.Collections.Generic;
using System.IO;
using Telegram.Bot.Types;

namespace SH_TelegramBot
{
    public static class SH_TelegramBotAuth
    {
        private static User user1 = new User();
        private static User user2 = new User();
        private static string UsersPath = @".\Users\users.sr";
        private static Dictionary<int, User> authUserList = new Dictionary<int, User>();

        static SH_TelegramBotAuth()
        {
            //TODO: check if file exists and catch exception if deSer fails 
            authUserList = JsonConvert.DeserializeObject<Dictionary<int, User>>(System.IO.File.ReadAllText(UsersPath));
        }

        public static bool CheckUserAuth(User from)
        {
            User user = new User();

            if (authUserList.TryGetValue(from.Id.GetHashCode(), out user))
            {
                if (user.LanguageCode == from.LanguageCode &&
                    user.FirstName == from.FirstName &&
                    user.LastName == from.LastName &&
                    user.Username == from.Username)

                    return true;
            }

            return false;
        }

        public static void AddUser(User user)
        {   
            Directory.CreateDirectory(Path.GetDirectoryName(UsersPath));

            if (user == null || user.Id == null)
                return; 

            authUserList.Add(user.Id.GetHashCode(), user);

            string serUsers = JsonConvert.SerializeObject(authUserList, Formatting.Indented);
            System.IO.File.WriteAllText(UsersPath, serUsers);
        }
    }
}
