#include "engine.h"

/* Two api functions you were not asked to implement
   returns the best price at the bid or ask */
t_price bestbid();
t_price bestask();
 
/* A market order which executes immediately at the best
   price possible
   IN: order: market order to add to book, price ignored
   OUT: order id of new order */
t_orderid market(t_order order) {
  order.price = is_ask(order.side) ? MAX_PRICE : MIN_PRICE;
  return limit(order);
}

/* Atomically replace an order on the book
   used by high frequency traders to ensure 
   that the new order and the old order cannot 
   both get executed
   IN: orderid: id of order to replace
   OUT: order id of new order */
t_orderid replace(t_orderid orderid, t_order order) {
  cancel(orderid); 
  return limit(order);
}

int printOrderBook(int numPriceLevels) {
	orderBookEntry_t *bookEntry;
	orderBookEntry_t *entry;
	pricePoint_t *ppEntry;

	ppEntry = pricePoints + bidMax + numPriceLevels;
	t_size size = 0;
	int count_ord;

	int l = 0; while (l < numPriceLevels) {
		if (bidMax + l >= MAX_PRICE) break;
		size = 0;
		count_ord = 0;
		bookEntry = ppEntry->listHead;
		while (bookEntry != NULL) {
			size += bookEntry->size;
			if (bookEntry->size > 0 ) count_ord++;
			bookEntry = bookEntry->next;
		}
		if (count_ord != ppEntry->numOrders) {
			printf("%d : %lu | %d vs %lu\n", (int) (bidMax + numPriceLevels - l), size, count_ord, ppEntry->numOrders);
			printf("size zero counter: %d\n", counter_of_size_zero);
			return 1;
		}
		if (size) {
			printf("%d : %lu | %d vs %lu\n", (int) (bidMax + numPriceLevels - l), size, count_ord, ppEntry->numOrders);
			if (l==4) printf("-----------------------------\n");
		}
		l++;
		ppEntry--;

	}

	//printf("----------------------\n");
	ppEntry = pricePoints + bidMax;

	l = 0; while (l < numPriceLevels) {
		size = 0;
		count_ord = 0;
		bookEntry = ppEntry->listHead;
		while (bookEntry != NULL) {
			size += bookEntry->size;
			if (bookEntry->size > 0 ) count_ord++;
			bookEntry = bookEntry->next;
		}
		if (count_ord != ppEntry->numOrders) {
			printf("%d : %lu | %d vs %lu\n", (int) (bidMax + l), size, count_ord, ppEntry->numOrders);
			printf("size zero counter: %d\n", counter_of_size_zero);
			return 1;
		}
		if (size) {
			printf("%d : %lu | %d vs %lu\n", (int) (bidMax + l), size, count_ord, ppEntry->numOrders);
		}
		l++;
		ppEntry--;
		if (bidMax - l  <= MIN_PRICE) break;
	}

	return 0;

}

/* An order type that is guaranteed to add liquidity
   used by market makes and rebate arbitrageurs
   IN: order: price will be ignored
       offset: number of ticks from side of NBBO
   OUT: orderid assigned to order */
t_orderid post(t_order order, t_price offset) {
  order.price = (is_ask(order.side) ? 
		 bestbid() - offset :
		 bestask() + offset);
  return limit(order);
}

