// Shared Libs aus der Semesterarbeit
#include <termPaperLib.h>
#include <concurrentLinkedList.h>

ConcurrentLinkedList *threadList;

void placeInt(int input) {

  log_info("Appended value = %d", input);

  // Input only has local scope!
  int *poin = &input;
  appendListElement(threadList, (void *) &poin, sizeof(int), "NoName");
}

void retriveInt(){
  void *firstElementCopy;

  getFirstListElement(threadList, &firstElementCopy) ;

  int *firstElementAsPayload = (int *) firstElementCopy;
  log_info("Got value = %d", *firstElementAsPayload );

  free(firstElementCopy);

  removeFirstListElement(threadList);
}

int main ( int argc, char *argv[] ) {


  // erstelle Start der linked List
  threadList = newList();
  log_debug("ConcurrentLinkedList: %p", threadList);

  placeInt(1);
  placeInt(2);
  placeInt(3);

  retriveInt();
  retriveInt();
  retriveInt();

  exit(0);
}
