#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <vector>
#include <string>

// #define debug(format, ...) fprintf(stderr, "%4d P%d " format, my_time, my_rank, ##__VA_ARGS__)

/*
 * Struct of the message
 */
struct Message {
  /*
   * Lamport clock value (time)
   */
  int time;
  /*
   * Process ID
   */
  int pid;
  /*
   * Message
   */
  std::string str;

  /*
   * Request struct constructor
   * @param t - Time
   * @param p - PID
   * @param m - message (string)
   */
  Message(int t, int p, std::string m ) {
    time = t;
    pid = p;
    str = m;
  }

    /*
   * Request struct constructor
   * @param t - Time
   * @param p - PID
   */
    Message(int t, int p) {
        time = t;
        pid = p;
        str = "";
    }

  /*
   * Override operator <
   * @param str - Request structure to compare
   */
  bool operator < (const Message& other) const {
    if (time < other.time) {
      return true;
    } else if (time == other.time && pid < other.pid) {
      return true;
    }

    return false;
  }
};

/*
 * Sort requests to critical section and other messages
 * @param std::vector &list - List with Requests to sort, required reference to sort original data
 */
void sort_requests(std::vector<Message> &list);

/*
 * Sort list of int.
 * @param std::vector &list
 */
void sort_list(std::vector<int> &list);

#endif
