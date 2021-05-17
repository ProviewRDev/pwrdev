/* rt_io_m_usb_agent.h -- I/O methods for class USB_Agent. */

#ifndef ra_io_m_mqtt_client_h
#define ra_io_m_mqtt_client_h

#if defined PWRE_CONF_MQTT

typedef enum {
  mqtt_eCon_NotConnected = 0,
  mqtt_eCon_Connected = 1,
  mqtt_eCon_WaitConnect = 2
} mqtt_eCon;

typedef struct {
  struct mosquitto *mosq;
  int is_subscriber;
  int is_publisher;
  mqtt_eCon connected;
  pthread_t loop_thread;
} io_sRackLocal;

#endif
#endif
