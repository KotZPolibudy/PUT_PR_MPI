#include <algorithm>
#include <vector>
#include "utils.h"

/*
 * Sort requests to critical section.
 * @param std::vector &list - List with Requests to sort, required reference to sort original data
 */
void sort_requests(std::vector<Message> &list) {
  std::sort(list.begin(), list.end());
}

/*
 * Sort list of int.
 * @param std::vector &list
 */
void sort_list(std::vector<int> &list) {
  std::sort(list.begin(), list.end());
}
