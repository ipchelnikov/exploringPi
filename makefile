gpio_led_switch : gpio_led_switch.cpp
	g++ gpio_led_switch.cpp -o led_switch

clean : led_switch
	rm led_switch
