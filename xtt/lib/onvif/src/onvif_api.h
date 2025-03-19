
typedef enum {
  onvif_mPTZ_pan = 1,
  onvif_mPTZ_tilt = 2,
  onvif_mPTZ_zoom = 4
} onvif_mPTZ;

class onvif_api {
  void *gsoap;
  pwr_tURL uri;
  char username[80];
  char passwd[80];
  void *PTZ_proxy;
  char manufacturer[80];
  char model[80];
  char firmware_version[80];
  int has_pantilt;
  int has_zoom;

 public:
  double x_range_min;
  double x_range_max;
  double y_range_min;
  double y_range_max;
  double zoom_range_min;
  double zoom_range_max;

  onvif_api(char *uri, char *username, char *passwd, pwr_tStatus *rsts);
  ~onvif_api();

  void report_error();
  void set_credentials();
  void move_absolute(double x, double y, double zoom, unsigned int mask);
  void move_relative(double x, double y, double zoom, unsigned int mask);
  void get_ptz(double *x, double *y, double *zoom);

};
