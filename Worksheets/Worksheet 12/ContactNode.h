/* Type code here */
#ifndef CONTACTNODE_H
#define CONTACTNODE_H

typedef struct ContactNodeStruct {
   char contactName[50];
   char contactPhoneNum[50];
   struct ContactNodeStruct* nextNodePointer;
} ContactNode;

void CreateContactNode(ContactNode* pointer, char nameInitial[], char phoneNumberInitial[], ContactNode* nextLocation);
void InsertContactAfter(ContactNode* thisNode, ContactNode* newNode); 
void PrintContactNode();
ContactNode* GetNextContact(); 

#endif