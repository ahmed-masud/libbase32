# libbase32

A C implementation of Douglas Crockford's base32 encoding.
The Crockford scheme is designed to be able to achieve the following:


  - Be human readable and machine readable.
  - Be compact. Humans have difficulty in manipulating long strings of arbitrary symbols.
  - Be error resistant. Entering the symbols must not require keyboarding gymnastics.
  - Be pronounceable. Humans should be able to accurately transmit the symbols to other humans using a telephone.


## Base32 Scheme
Canonical information about Crockford-encoding scheme is found at: 

[http://www.crockford.com/wrmg/base32.html](http://www.crockford.com/wrmg/base32.html)

##IMPORTANT NOTE
This library is maintained by me, please don't bother anyone else about it. Especially not Douglas Crockford. 


# API

```C
size_t base32enc(char *dest, const void *src, size_t s_len);
```

Encodes `s_len` bytes from `src` to base32 and stores the result in `dest`.  
returns size of `dest`

```C
size_t base32dec(void *dest, size_t dest_len, const char *src);
```

Decodes `src` to `dest` for <i>up to</i> `dest_len` bytes.


```C
size_t base32encsize(size_t count);
```

Returns estimate of number of bytes required to encode `count` raw bytes

```C
size_t base32decsize(size_t count);
```

Returns estimate of number of bytes required to decode, `count` long base32 string
