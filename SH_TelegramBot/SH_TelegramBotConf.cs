#region Assembly Telegram.Bot, Version=12.0.0.0, Culture=neutral, PublicKeyToken=null
// H:\Projects\SmartHome\MySmartHome\packages\Telegram.Bot.12.0.0-beta-03\lib\netstandard1.1\Telegram.Bot.dll
#endregion

using Newtonsoft.Json;



namespace SH_TelegramBot
{
    [JsonObject(MemberSerialization.OptIn)]
    class SH_TelegramBotConf
    {
        private static string confPath = @".\ShBotConf.conf"; 

        [JsonProperty(PropertyName = "id", Required = Required.Always)]
        public string BotId { get; set; }

        [JsonProperty(PropertyName = "admin", Required = Required.AllowNull)]
        public string AdminUser { get; set; }
        public SH_TelegramBotConf() {}

        public void Init()
        {
            SH_TelegramBotConf conf = JsonConvert.DeserializeObject<SH_TelegramBotConf>(System.IO.File.ReadAllText(confPath));

            BotId = conf.BotId;

            AdminUser = conf.AdminUser;
        }
    }
}
