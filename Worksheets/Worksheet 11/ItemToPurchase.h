#ifndef ITEM_TO_PURCHASE_H
#define ITEM_TO_PURCHASE_H

/* Type your code here */
typedef struct ItemToPurchase_struct {
   char itemName[50];
   int itemPrice;
   int itemQuantity;
} ItemToPurchase;
  
void MakeItemBlank(ItemToPurchase* item);
void printItemCost(ItemToPurchase item);

#endif