# File Archiver
Simple file archiver developed on the programming 2 class at UFPR.

A file archiver is a program that groups multiple files inside a single file(an archive), similar to [tar](https://en.wikipedia.org/wiki/Tar_(computing)), without compressing any data.

## How to use
To compile:
  * run ```make```

Format:  
  * ```vina++ <option> <archive> [member1 member2 ...]```

Options:
* -i: inserts one or more members into the archive; if that member already exists, it's substituted, new members are added in order at the end of the archive.
* -a: same as -i, but it only substitutes a member if it is newer than the archived one.
* -m target: moves the command line "member" to exactly after the "target" file inside the "archive".
* -x: extracts the members from the archive, if no members are specified, all are extracted.
* -r: removes the specified members from the archive.
* -c: lists the contents of the archive in order, including the member's information (name, UID, permissions, size and modification date).
* -h: prints a message with the available options.

### Structure
![image](https://github.com/user-attachments/assets/133319e3-e3cb-4bcf-9c55-469fbe03fbca)  
Offset:
* Contains the location of the information table.
  
Data section:
* After the offset, all the files are stored one after another, without information between them.
  
Information table:
* Contains all information about all the files in order.


#### Extra details:
* Buffer limit of 1024 bytes to manipulate members data.
* Reading and writing are done only in binary mode with [fread()](https://man7.org/linux/man-pages/man3/fread.3.html) and fwrite().
* If something goes wrong, the program shows a message explaining the error.
