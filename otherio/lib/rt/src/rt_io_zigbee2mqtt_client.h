/* rt_io_m_usb_agent.h -- I/O methods for class USB_Agent. */

#ifndef ra_io_m_zigbee2mqtt_client_h
#define ra_io_m_zigbee2mqtt_client_h

#if defined PWRE_CONF_MQTT

typedef enum {
  z2m_eCon_NotConnected = 0,
  z2m_eCon_Connected = 1,
  z2m_eCon_WaitConnect = 2
} z2m_eCon;

typedef struct {
  struct mosquitto *mosq;
  int is_subscriber;
  int is_publisher;
  z2m_eCon connected;
  pthread_t loop_thread;
  char subscribe_topic[80];
} io_sRackLocal;

typedef struct {
  char subscribe_topic[80];
  char publish_topic[80];
  pwr_tFloat32 *transition_p;
  pwr_tString80 *str_old;
} io_sCardLocal;

#endif
#endif
