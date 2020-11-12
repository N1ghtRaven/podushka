# podushka

Simple library for parse/serialize, decode/encode GSM PDU pockets (sms-deliver/sms-submit).

## Usage
### Dependencies
* [Criterion](https://github.com/Snaipe/Criterion) - used only for testing

### Build
```sh
$ cd podushka
$ make
```

### Example
There are also examples of using the library inside.
* manual_decoder - ask from user encoded PDU string and print decoded dump

## Credits
The following repositories and resources were used during development:

* [Spec](http://embeddedpro.ucoz.ru/app_notes/send_short_SMS/PDU1.pdf)

* [Online Decoder](https://www.diafaan.com/sms-tutorials/gsm-modem-tutorial/online-sms-pdu-decoder)

* [ucs2-utf8](https://github.com/smoothwind/ucs2-utf8)

* [gsm7bit](https://github.com/vbs100/gsm7bit)

* [7bit_encoding](https://github.com/RiccardoSottini/7bit_encoding)

  


