# Columnar Binary JSON Books

This directory (`book`) contains the Carbon Specification document, and a developer manual for [Karbonit](https://github.com/karbonitlabs/karbonit) (including the Carbon API).

## Specification 

The sources of current Carbon Specification draft are located in the `carbonspec/latest` directory. 
The specification is formatted with  markdown, and built with [MdBook](https://github.com/rust-lang-nursery/mdBook).

To build the specification book, type in your bash
```
$ cd carbonspec/latest
$ mdbook build
```

> You find the latest stable snapshot on [carbonspec.org](http://www.carbonspec.org)

## Karbonit Developer Documentation 

The sources of current Karbonit Developer Documentation are located in the `karbonit-devdoc/latest` directory. 
The Developer Documentation is formatted with  markdown, and built with [MdBook](https://github.com/rust-lang-nursery/mdBook).

To build the specification book, type in your bash
```
$ cd karbonit-devdoc/latest
$ mdbook build
```

> You find the latest stable snapshot on [karbonit-devdocs.org](http://www.karbonit-devdocs.org)