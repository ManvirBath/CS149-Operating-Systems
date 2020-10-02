#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "ContactNode.h"

int main(void){
    /* Type code here */
   ContactNode* head = NULL;
   ContactNode* current = NULL;
   ContactNode* next1 = NULL;
   ContactNode* next2 = NULL;

   char name[50] = "";
   char phone[50] = "";
   
   for(int i = 1; i <= 3; i++) {
      printf("Person %d\nEnter name:\n", i);
      scanf("%[^\n]%*c", name);
      printf("Enter phone number:\n");
      scanf("%[^\n]%*c", phone);
      
      if(i == 1) {
         head = (ContactNode*)malloc(sizeof(ContactNode));
         CreateContactNode(head, name, phone, NULL);
      } else if(i == 2){
         next1 = (ContactNode*)malloc(sizeof(ContactNode));
         CreateContactNode(next1, name, phone, NULL);
         InsertContactAfter(head, next1);
      } else {
         next2 = (ContactNode*)malloc(sizeof(ContactNode));
         CreateContactNode(next2, name, phone, NULL);
         InsertContactAfter(next1, next2);
      }
      printf("You entered: %s, %s\n\n", name, phone); // Print node
   }
   printf("CONTACT LIST\n"); // For formatting
   for(current = head; current != NULL; current = GetNextContact(current)) PrintContactNode(current); // Print list
   return 0;
}