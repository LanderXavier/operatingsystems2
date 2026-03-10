#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>

static int led_gpio = 17;
static int btn_gpio = 27;
module_param(led_gpio, int, 0644);
module_param(btn_gpio, int, 0644);

static int btn_irq = -1;
static bool led_on;
static unsigned long last_jiffies;

static irqreturn_t btn_isr(int irq, void *dev_id)
{
    unsigned long now = jiffies;
    
    // simple debounce: ignore presses within 200ms
    if (time_before(now, last_jiffies + msecs_to_jiffies(200)))
        return IRQ_HANDLED;
    
    last_jiffies = now;
    led_on = !led_on;
    gpio_set_value(led_gpio, led_on);
    pr_info("gpio_btn_led: button irq, led=%d\n", led_on);
    
    return IRQ_HANDLED;
}

static int __init gpio_btn_led_init(void)
{
    int ret;
    
    ret = gpio_request(led_gpio, "led_gpio");
    if (ret) return ret;
    
    ret = gpio_direction_output(led_gpio, 0);
    if (ret) goto err_led;
    
    ret = gpio_request(btn_gpio, "btn_gpio");
    if (ret) goto err_led;
    
    ret = gpio_direction_input(btn_gpio);
    if (ret) goto err_btn;

    btn_irq = gpio_to_irq(btn_gpio);
    if (btn_irq < 0) { ret = btn_irq; goto err_btn; }
    
    ret = request_irq(btn_irq, btn_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "btn_irq", NULL);
    if (ret) goto err_btn;
    
    pr_info("gpio_btn_led: loaded (led_gpio=%d btn_gpio=%d irq=%d)\n", led_gpio, btn_gpio, btn_irq);
    return 0;

err_btn:
    gpio_free(btn_gpio);
err_led:
    gpio_free(led_gpio);
    return ret;
}

static void __exit gpio_btn_led_exit(void)
{
    if (btn_irq >= 0)
        free_irq(btn_irq, NULL);
    gpio_set_value(led_gpio, 0);
    gpio_free(btn_gpio);
    gpio_free(led_gpio);
    pr_info("gpio_btn_led: unloaded\n");
}

module_init(gpio_btn_led_init);
module_exit(gpio_btn_led_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("GPIO button toggles LED using IRQ");
