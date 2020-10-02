#include<stdio.h>
#include<string.h>

#include "ItemToPurchase.h"

int main(void) {
 
   /* Type your code here */  
   int i, total;
   char c;
   const int NUM_ITEMS=2;
   ItemToPurchase firstItem;
   ItemToPurchase secondItem;
   
   for (i = 0; i < NUM_ITEMS; i++) {
      if (i==0){
         printf("Item %d\n", i+1);
         printf("Enter the item name:\n");
         fgets((firstItem.itemName), 50, stdin);
         fflush(stdin);
         int len = strlen(firstItem.itemName);
         firstItem.itemName[len-1] = '\0';
         printf("Enter the item price:\n");
         scanf("%d", &(firstItem.itemPrice));
         printf("Enter the item quantity:\n\n");
         scanf("%d", &(firstItem.itemQuantity));
   }
   if (i == 1) {
      printf("Item %d\n", i+1);
      printf("Enter the item name:\n");
      fgets((secondItem.itemName),50,stdin);
      fflush(stdin);
      int len = strlen(secondItem.itemName);
      secondItem.itemName[len-1] = '\0'; // for removing new line
      printf("Enter the item price:\n");
      scanf("%d", &(secondItem.itemPrice));
      printf("Enter the item quantity:\n");
      printf("\n");
      scanf("%d", &(secondItem.itemQuantity));
   }
   while ((c=getchar())!=EOF&& c != '\n');
   }
   total = (firstItem.itemPrice * firstItem.itemQuantity) + (secondItem.itemPrice*secondItem.itemQuantity);
   printf("TOTAL COST\n");
   printItemCost(firstItem);
   printItemCost(secondItem);
   printf("\nTotal: $%d\n", total);
   return 0;
}