# Exercise 3

## The Task

Write a device driver for a character device which implements a simple way of message passing. The kernel maintains a list of messages. To limit memory usage, we impose a limit of `4KiB = 4*1024 bytes` for each message, and also impose a limit of the size of all messages, which is initially `2MiB = 2*1024*1024 bytes`.

Your device driver should perform the following operations:
* Creating the device, which has to be `/dev/opsysmem`, creates an empty list of messages.
* Removing the device deallocates all messages and removes the list of messages.
* Reading from the device returns one message, and removes this
message from the kernel list. If the list of messages is empty, the reader returns `-EAGAIN`.
* Writing to the device stores the message in kernel space and adds it to the list if the message is below the maximum size, and the limit of the size of all messages wouldn't be surpassed with this message. If the message is too big, `-EINVAL` is returned, and if the limit of the size of all messages was surpassed, `-EAGAIN` is returned.
* You should also provide an ioctl which sets a new maximum size of all messages. This operation should succeed only if the new maximum is bigger than the size of all messages currently held. The ioctl number for this operation should be 0, and the ioctl parameter should be the new maximum size. The ioctl should return 0 on success and `-EINVAL` on failure.
* The kernel module which implements this driver must be called
`charDeviceDriver.ko`.
* Your solution must not use the kernel fifo.

You need to ensure that your code deals with multiple attempts at reading and writing at the same time. Your code should minimise the time spent in critical sections. The reader should obtain the messages in the same order as they were written.

## Extra Task (Bonus)

For 5 bonus marks you should implement blocking reads and writes: instead of returning `-EAGAIN` when reading and writing from the device, you should block the reader until a message is available. Similarly, you should block the writer until there is room for the message (in case of the writer). For the available kernel mechanisms to achieve blocking, see the section 'Wait queues and Wake events' in the device driver documentation (https://www.linuxtv.org/downloads/v4l-dvb-internals/device-drivers/ch01s04.html). If you do this bonus part you will need to produce two kernel modules, one named `charDeviceDriver.ko` for the non-bonus part, and one named `charDeviceDriverBlocking.ko` for the bonus part. You may assume that only one of these two modules will be loaded at any given time.

## Testing
Do not modify or add files in the repository that start with `test`, since we use those files for scripts for testing. The test script expects the kernel module at module loading time to add the major number to `/var/log/syslog`, as done in the example code. The scripts used for marking will expect this output as well. A simple test script is provided here (https://canvas.bham.ac.uk/files/8193253/download?download_frd=1).

## Submission
You should put all your files into a directory named exercise3 in your project on the School's git server. The The directory exercise3 must be at the top level of your project for the marking scripts to work. We will run the command make in the directory exercise3 in order to obtain all required binaries. Do not use any files or directory with the prefix "test", since we will use such files for running tests. Submission consists of pushing your solution to your project as specified above; the last submission before the submission deadline has ended counts as your submission. It is worth trying out submission well before the deadline even if the solution is not correct yet.

For marking we will use additional, more advanced, test scripts which check whether your program satisfies the specification. If the provided test scripts fail, all the more advanced test scripts are likely to fail as well. Any code which does not compile on the virtual machine provided will be awarded 0 marks and not be reviewed.

## Useful Links

* [Introduction to writing linux kernel modules, part 1](http://derekmolloy.ie/writing-a-linux-kernel-module-part-1-introduction)
* [Introduction to writing linux kernel modules, part 2](http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device)
* [Linux kernel API](https://www.kernel.org/doc/htmldocs/kernel-api)
* [Browsing linux kernel source](https://elixir.bootlin.com/linux/v4.4.93/source)
