# Columnar Binary JSON Books

This directory (`book`) contains the Carbon Specification document, and a manual on the [libcarbon](https://github.com/protolabs/libcarbon) library.

## Specification 

The sources of current Carbon Specification draft are located in the `spec/latest` directory. The specification is formatted with  markdown, and built with [MdBook](https://github.com/rust-lang-nursery/mdBook).

To build the specification book, type in your bash
```
$ cd spec/latest
$ mdbook build
```