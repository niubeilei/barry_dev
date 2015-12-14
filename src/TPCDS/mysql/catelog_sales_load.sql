LOAD DATA LOCAL INFILE 'catalog_sales_1_12.dat'
INTO TABLE catalog_sales
FIELDS TERMINATED BY '|'
LINES TERMINATED BY '\n'
(@cs_sold_date_sk, @cs_sold_time_sk, @cs_ship_date_sk, @cs_bill_customer_sk, @cs_bill_cdemo_sk, @cs_bill_hdemo_sk, @cs_bill_addr_sk, @cs_ship_customer_sk, @cs_ship_cdemo_sk, @cs_ship_hdemo_sk, @cs_ship_addr_sk, @cs_call_center_sk, @cs_catalog_page_sk, @cs_ship_mode_sk, @cs_warehouse_sk, cs_item_sk, @cs_promo_sk, cs_order_number, cs_quantity, cs_wholesale_cost, cs_list_price, cs_sales_price, cs_ext_discount_amt, cs_ext_sales_price, cs_ext_wholesale_cost, cs_ext_list_price, cs_ext_tax, cs_coupon_amt, cs_ext_ship_cost, cs_net_paid, cs_net_paid_inc_tax, cs_net_paid_inc_ship, cs_net_paid_inc_ship_tax, cs_net_profit)
SET
cs_sold_date_sk     = NULLIF(@cs_sold_date_sk, ''),
cs_sold_time_sk     = NULLIF(@cs_sold_time_sk, ''),
cs_ship_date_sk     = NULLIF(@cs_ship_date_sk, ''),
cs_bill_customer_sk = NULLIF(@cs_bill_customer_sk, ''),
cs_bill_cdemo_sk    = NULLIF(@cs_bill_cdemo_sk, ''),
cs_bill_hdemo_sk    = NULLIF(@cs_bill_hdemo_sk, ''),
cs_bill_addr_sk     = NULLIF(@cs_bill_addr_sk, ''),
cs_ship_customer_sk = NULLIF(@cs_ship_customer_sk, ''),
cs_ship_cdemo_sk    = NULLIF(@cs_ship_cdemo_sk, ''),
cs_ship_hdemo_sk    = NULLIF(@cs_ship_hdemo_sk, ''),
cs_ship_addr_sk     = NULLIF(@cs_ship_addr_sk, ''),
cs_call_center_sk   = NULLIF(@cs_call_center_sk, ''),
cs_catalog_page_sk  = NULLIF(@cs_catalog_page_sk, ''),
cs_ship_mode_sk     = NULLIF(@cs_ship_mode_sk, ''),
cs_warehouse_sk     = NULLIF(@cs_warehouse_sk, ''),
cs_promo_sk         = NULLIF(@cs_promo_sk, '');
