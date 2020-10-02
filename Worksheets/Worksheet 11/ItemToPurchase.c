#include<stdio.h>
#include<string.h>
#include "ItemToPurchase.h"

/* Type your code here */
void MakeItemBlank(ItemToPurchase* item){
   strcpy((*item).itemName, "none"); 
   (*item).itemPrice=0;
   (*item).itemQuantity=0;
}

void printItemCost(ItemToPurchase item){
   printf("%s %d @ $%d = $%d\n", item.itemName, item.itemQuantity, item.itemPrice, (item.itemPrice * item.itemQuantity));
}
