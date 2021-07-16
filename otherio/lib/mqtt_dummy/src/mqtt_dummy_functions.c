/*
  Dummy for mosquitto
 */
#if defined PWRE_CONF_MQTT

#include <mosquitto.h>

int mosquitto_topic_matches_sub(const char *sub, const char *topic, bool *result) 
{
  return -1;
}
const char *mosquitto_connack_string(int connack_code)
{
  static char s[] = "";
  return s;
}
int mosquitto_subscribe(struct mosquitto *mosq, int *mid, const char *sub, int qos)
{
  return -1;
}
struct mosquitto *mosquitto_new(const char *id, bool clean_session, void *obj)
{
  return NULL;
}
void mosquitto_connect_callback_set(struct mosquitto *mosq, void (*on_connect)(struct mosquitto *, void *, int))
{
}
void mosquitto_message_callback_set(struct mosquitto *mosq, void (*on_message)(struct mosquitto *, void *, const struct mosquitto_message *))
{
}
int mosquitto_username_pw_set(struct mosquitto *mosq, const char *username, const char *password)
{
  return -1;
}
int mosquitto_reconnect(struct mosquitto *mosq)
{
  return -1;
}
int mosquitto_loop(struct mosquitto *mosq, int timeout, int max_packets)
{
  return -1;
}
int mosquitto_publish(struct mosquitto *mosq, int *mid, const char *topic, int payloadlen, const void *payload, int qos, bool retain)
{
  return -1;
}
int mosquitto_connect(struct mosquitto *mosq, const char *host, int port, int keepalive)
{
  return -1;
}
int mosquitto_disconnect(struct mosquitto *mosq)
{
  return -1;
}
#endif

