#ifndef IPEGA_H
#define IPEGA_H

#include <bluetooth/bluetooth.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

#include <atomic>
#include <string>
#include <thread>
#include <tuple>

/******************************************************************************/
class IpegaButton {
 public:
  IpegaButton() : state(false) {}

 public:
  std::atomic<bool> state;
};
/******************************************************************************/
class IpegaTrigger {
 public:
  IpegaTrigger() : state(false), value(0) {}

 public:
  std::atomic<bool> state;
  std::atomic<int> value;
};
/******************************************************************************/
class IpegaJoy {
 public:
  IpegaJoy() : state(false), x_value(0), y_value(0) {}

 public:
  std::atomic<bool> state;
  std::atomic<int> x_value;
  std::atomic<int> y_value;
};
/******************************************************************************/
class Ipega {
 public:
  explicit Ipega(const std::string& path = "/dev/input/js0");
  ~Ipega();

 public:
  void printState();
  bool getStartButton();
  bool getSelectButton();
  bool getHomeButton();
  bool getDpadLeft();
  bool getDpadRight();
  bool getDpadUp();
  bool getDpadDown();
  bool getAButton();
  bool getBButton();
  bool getXButton();
  bool getYButton();
  bool getBumperLeft();
  bool getBumperRight();
  bool getRightStickButton();
  bool getLeftStickButton();
  std::tuple<bool, int> getTriggerLeft();
  std::tuple<bool, int> getTriggerRight();
  std::tuple<int, int> getLeftStick();
  std::tuple<int, int> getRightStick();

 private:
  void spin();
  void spinOnce();
  void printRaw();

  void parseData(uint8_t* data, int bytes);
  void decodeData(uint8_t* data);

  void decodeSingleData(uint8_t* data);
  void decodeLineData(uint8_t* data);

  void decodeDpadVertical(uint8_t* data);
  void decodeDpadHorizontal(uint8_t* data);
  void decodeJoyLeftVertical(uint8_t* data);
  void decodeJoyLeftHorizontal(uint8_t* data);
  void decodeJoyRightVertical(uint8_t* data);
  void decodeJoyRightHorizontal(uint8_t* data);
  void decodeTriggerLeft(uint8_t* data);
  void decodeTriggerRight(uint8_t* data);

 private:
  std::thread worker;
  std::atomic<bool> stopped;

  std::string path;

  fd_set read_fds;  // read_file_descriptor_set
  struct timeval timeout;
  const long timeout_u = 100000;  // 100ms
  int file_descriptor;
  int bytes_read;
  uint8_t buffer[20];

 private:
  // func buttons
  IpegaButton start_button;
  IpegaButton select_button;
  IpegaButton home_button;
  // dpad
  IpegaButton dpad_left;
  IpegaButton dpad_right;
  IpegaButton dpad_up;
  IpegaButton dpad_down;
  // ABXY
  IpegaButton A;
  IpegaButton B;
  IpegaButton X;
  IpegaButton Y;
  // Bumpers
  IpegaButton bump_left;
  IpegaButton bump_right;
  // Triggers
  IpegaTrigger trig_left;
  IpegaTrigger trig_right;
  // Joysticks
  IpegaJoy joy_left;
  IpegaJoy joy_right;
  // Sticks buttons
  IpegaButton lsb;  // right stick button
  IpegaButton rsb;  // left stick button
};
/******************************************************************************/
#endif  // IPEGA_H