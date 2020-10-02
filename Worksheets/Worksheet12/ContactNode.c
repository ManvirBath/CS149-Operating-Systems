/* Type code here */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"ContactNode.h"

void CreateContactNode(ContactNode* thisNode, char newName[], char newNumber[], ContactNode* nextNode){
   strcpy(thisNode->contactName, newName);
   strcpy(thisNode->contactPhoneNum, newNumber);
   thisNode->nextNodePointer = nextNode;
   return;
}
void PrintContactNode(ContactNode* thisNode){
   printf("Name: %s\nPhone number: %s\n\n", thisNode->contactName, thisNode->contactPhoneNum);
   return;
}
void InsertContactAfter(ContactNode* thisNode, ContactNode* newNode){
   ContactNode* tmp = NULL;
   tmp = thisNode->nextNodePointer;
   thisNode->nextNodePointer = newNode;
   newNode->nextNodePointer = tmp;
   return;
}
ContactNode* GetNextContact(ContactNode* thisNode){
   return thisNode->nextNodePointer;   
}
