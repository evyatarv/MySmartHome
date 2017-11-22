
// ============= COMPILE CONF. =============
//set device (only one)
#define SONOFF_DUAL 1
//#define SONOFF_SINGLE 1

//set device app
#ifdef SONOFF_DUAL
//#define SHUTTER 1
#define SWITCHS 1
#endif //SONOFF_DUAL

// ============= COMPILE END =============



// ============ BASE. =============
#define HALF_SECOND   (500)
#define SECOND        (HALF_SECOND * 2)
#define TWO_SEC       (SECOND * 2)
#define FOUR_SEC      (SECOND * 4)
// ============ BASE ENDS. ========



// ============ DEVICE CONF. =============
#define UART_CONTROLER_SPEED          (19200)   //uart speed to relays micro-controller
#define UART_CONSOLE_SPEED            (115200)

#define DEVICE_STATUS_ERR_DELAY_TIME  (100)
#define DEVICE_ERR_MESSAGE_PULL_TIME  (SECOND)
#define DEVICE_RESTART_TIMEOUT        (40)
#define DEIVCE_DELAY_AFTER_SER_WRITE  (HALF_SECOND)
unsigned int restart_counter = 0;
// ============= DEVICE ENDS =============



// ============ SHUTTER APP CONF. =============
#define SHUTTER_FULL_DURATION_LENGTH    (1) //1 byte in sec
#define SHUTTER_CRACKS_DURATION_LENGTH  (1) //1 byte in sec
// ============= SHUTTER APP ENDS =============



// ============ WIFI CONF. ============
#define WIFI_STATUS_PULL_INTERVAL (HALF_SECOND)
#define WIFI_CONNECTION_TIMEOUT   (120)
#define WIFI_MAX_SSID_LENGTH      (32)
#define WIFI_MAX_PASSWORD_LENGTH  (32)
#define WIFI_MAX_IPV4_STR_LENGTH  (15)
#define WIFI_MIN_IPV4_STR_LENGTH  (7)
#define WIFI_IPV4_RAW_DATA_LENGTH (4)

enum WIFI_CONF_NUM{ SSID_NAME=0, PASSWORD, GATEWAY, MASK_IP, DEVICE_IP, MAX_CONF_LEN };
const char* WIFI_CONF[] = { "ssid", "password", "gateway", "mask_ip", "device_ip" };
String WIFI_CURRENT_SSID_NAME = WIFI_DEFAULT_SSID_NAME;
String WIFI_CURRENT_SSID_PASSWORD = WIFI_DEFAULT_SSID_PASSWORD;
IPAddress DEFAULT_GATEWAY;
IPAddress SUB_MASK_IP;
IPAddress DEV_IP;
// ============= WIFI ENDS =============



// ============ HTTP SERVER CONF. ============
#define HTTP_MAX_USER_AUTH_LENGTH     (32)
#define HTTP_MAX_PASSWORD_AUTH_LENGTH (32)
#define HTTP_RESPONSE_DELAY_TIME      (20)

String HTTP_RETURN_WEB_PAGE = "";

#define HTTP_RESPONSE_OK              (200)
#define HTTP_RESPONSE_PAGE_NOT_FOUND  (400)
// ============= HTTP SERVER ENDS =============



// ============ EEPROM CONF. ============
#define EEPROM_SIZE                             (256)
#define EEPROM_DATA_LENGTH_OFFSET               (1)
#define EEPROM_FIRST_BOOT_STR                   ("OK")
#define EEPROM_FIRST_BOOT_STR_LEN               (2)
#define EEPROM_WIFI_SSID_OFFSET                 (2)
#define EEPROM_WIFI_PASSWORD_OFFSET             (EEPROM_WIFI_SSID_OFFSET + EEPROM_DATA_LENGTH_OFFSET + WIFI_MAX_SSID_LENGTH)
#define EEPROM_WIFI_DEVICE_IP_OFFSET            (EEPROM_WIFI_PASSWORD_OFFSET + EEPROM_DATA_LENGTH_OFFSET + WIFI_MAX_PASSWORD_LENGTH)
#define EEPROM_WIFI_SUBMASK_OFFSET              (EEPROM_WIFI_DEVICE_IP_OFFSET + EEPROM_DATA_LENGTH_OFFSET + WIFI_IPV4_RAW_DATA_LENGTH)
#define EEPROM_WIFI_DEFUALT_GATEWAY_OFFSET      (EEPROM_WIFI_SUBMASK_OFFSET + EEPROM_DATA_LENGTH_OFFSET + WIFI_IPV4_RAW_DATA_LENGTH)
#define EEPROM_HTTP_USER_AUTH_OFFSET            (EEPROM_WIFI_DEFUALT_GATEWAY_OFFSET + EEPROM_DATA_LENGTH_OFFSET + WIFI_IPV4_RAW_DATA_LENGTH)
#define EEPROM_HTTP_PASSWORD_AUTH_OFFSET        (EEPROM_HTTP_USER_AUTH_OFFSET + EEPROM_DATA_LENGTH_OFFSET + HTTP_MAX_USER_AUTH_LENGTH)
#define EEPROM_SHUTTER_FULL_DURATION_OFFSET     (EEPROM_HTTP_PASSWORD_AUTH_OFFSET + EEPROM_DATA_LENGTH_OFFSET + SHUTTER_FULL_DURATION_LENGTH)
#define EEPROM_SHUTTER_CRACKS_DURATION_OFFSET   (EEPROM_SHUTTER_FULL_DURATION_OFFSET + EEPROM_DATA_LENGTH_OFFSET +  SHUTTER_CRACKS_DURATION_LENGTH)

// ============= EEPROM ENDS =============



// ============ MACROS. ============
#define SET_BIT(x, index)   (x |= 1 << index)
#define CLEAR_BIT(x, index) (x &= ~(1 << index))
// ============ MACROS END. ============



// ============== DEVICE API ==============
enum relay_cmd
{
  relay_off,
  relay_on,
  all_relays_on,
  all_relays_off,
  move_full,
  move_cracks,
  move_half,
  move_duration,
};

enum relays
{
  first_relay = 0,
  second_relay,
  both_relays,
};

enum led_cmd
{
  led_off,
  led_on,
  all_leds_on,
  all_leds_off,
};

enum led_index {
  first_led = 0,
  second_led
};

void device_func_place_holder(){Serial.println("FUNC PLACE HOLDER :( ");}

typedef void (*device_relay)(int index, int cmd);

typedef void (*device_reset)();
typedef void (*device_indicator)();
typedef void (*device_led)(int index, int cmd);
typedef void (*device_err)();
typedef void (*device_init)();

typedef struct _device_api
{
  device_init init = NULL;
  
  device_relay relay = NULL;
  device_led  led = NULL;
  device_reset do_device_could_reset = device_func_place_holder;
  device_indicator do_indicate = device_func_place_holder;
  device_err enter_err_mode = device_func_place_holder;
}device_api;

device_api* dev_api;
// ============= DEVICE APIS ENDS =============



// =============== FUNC PROTOTYPES ================
void prepare_gpios(); 
void prepare_serial();
void init_eeprom();
void load_config(bool set_static_ip);
bool connect_wifi_network(bool set_static_ip);
void init_http_server();
void init_device();

void set_device_api(device_api const &api);
// ============= FUNC PROTOTYPES ENDS =============


// ============== SETUP ==============
void componentes_setup()
{
  bool set_static_ip = false; 

  init_eeprom();
  
  init_device();

  load_config(&set_static_ip);

  if (!connect_wifi_network(set_static_ip))
    device_err();

  init_http_server();
}
// ============= SETUP ENDS =============
