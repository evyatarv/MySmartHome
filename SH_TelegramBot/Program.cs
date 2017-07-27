using System;
using System.Threading.Tasks;
using Telegram.Bot;
using Telegram.Bot.Args;
using Telegram.Bot.Types;
using Telegram.Bot.Types.Enums;
using Telegram.Bot.Types.ReplyMarkups;
using SH_UnmanagedWrapper;

namespace SH_TelegramBot
{
    public enum LIGHT_SELECTION{ FRONT_YARD=1, BACK_YARD=2 } 

    class Program
    {
        private static readonly string NOT_AUTHORIZED_STR = "User not Authorized";

        private static TelegramBotClient bot;
        private static SH_TelegramBotConf botConf = new SH_TelegramBotConf();

        private static string authName = "evyatar";
        private static string autPass = "12345";
        private static string address = "10.12.231.14";
        private static IntPtr context = SH_Unmanaged.CreateContext(0, -1, authName, authName.Length, autPass, autPass.Length);
        private static IntPtr sh_switch = SH_Unmanaged.CreateSwitch(2, address, address.Length);

        static void Main(string[] args)
        {
            
            botConf.Init();

            bot = new TelegramBotClient(botConf.BotId);

            bot.OnCallbackQuery += BotOnCallbackQueryReceived;
            bot.OnMessage += BotOnMessageReceived;
            bot.OnMessageEdited += BotOnMessageReceived;
            bot.OnInlineResultChosen += BotOnChosenInlineResultReceived;
            bot.OnReceiveError += BotOnReceiveError;

            var me = bot.GetMeAsync().Result;

            Console.Title = me.Username;

            bot.StartReceiving();
            Console.ReadLine();
            bot.StopReceiving();
        }

        private static void BotOnReceiveError(object sender, ReceiveErrorEventArgs receiveErrorEventArgs)
        {
            //Debugger.Break();
            Console.WriteLine("in: BotOnReceiveError");
            Console.WriteLine(sender);
            Console.WriteLine(receiveErrorEventArgs.ApiRequestException.Message);
        }

        private static void BotOnChosenInlineResultReceived(object sender, ChosenInlineResultEventArgs chosenInlineResultEventArgs)
        {
            Console.WriteLine($"Received choosen inline result: {chosenInlineResultEventArgs.ChosenInlineResult.ResultId}");
        }

        private static async void BotOnMessageReceived(object sender, MessageEventArgs messageEventArgs)
        {
            var message = messageEventArgs.Message;

            if (message == null || message.From == null || message.Type != MessageType.TextMessage) return;

            if (!SH_TelegramBotAuth.CheckUserAuth(message.From))
            {
                await bot.SendTextMessageAsync(message.Chat.Id, NOT_AUTHORIZED_STR);
                return;
            }

            await bot.SendChatActionAsync(message.Chat.Id, ChatAction.Typing);

            await SendLightButtons(message);
        }

        private static async Task SendLightButtons(Message message)
        {
            var keyboard = new InlineKeyboardMarkup(new[]
                        {
                    new[] // first row
                    {
                        new InlineKeyboardButton("Front Yard", "1"),
                        new InlineKeyboardButton("Back Yard", "2"),
                    },
                    new[] // second row                 
                    {
                        new InlineKeyboardButton("RESERVE_1", "3"),
                        new InlineKeyboardButton("RESERVE_2", "dfsd"),
                    }
                });

            await Task.Delay(500); // simulate longer running task

            await bot.SendTextMessageAsync(message.Chat.Id, "Choose",
                replyMarkup: keyboard);
        }

        private static async void BotOnCallbackQueryReceived(object sender, CallbackQueryEventArgs callbackQueryEventArgs)
        {
            int lightSelection;
            string retString;

            try
            {
                if (!SH_TelegramBotAuth.CheckUserAuth(callbackQueryEventArgs.CallbackQuery.From))
                {
                    retString = NOT_AUTHORIZED_STR;
                }
                else
                {
                    lightSelection = Convert.ToInt32(callbackQueryEventArgs.CallbackQuery.Data);

                    if (Enum.IsDefined(typeof(LIGHT_SELECTION), lightSelection))
                    {
                        TurnLight((LIGHT_SELECTION)lightSelection);
                        retString = "OK";
                    }
                    else
                    {
                        Console.WriteLine("Wrong selection {0}", lightSelection);
                        retString = "Wrong selection";
                    }
                }

                await bot.AnswerCallbackQueryAsync(callbackQueryEventArgs.CallbackQuery.Id, retString);
            }
            catch (Exception e)
            {
                Console.WriteLine("in: BotOnCallbackQueryReceived");
                Console.WriteLine(e.Message);

                return;
            }
        }

        private static void TurnLight(LIGHT_SELECTION lightSelection)
        {
            SH_Unmanaged.Alternate(context, sh_switch);
        }
    }
}
