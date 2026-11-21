# Colorless Color

## What is this?

This is a program I wrote in C++ that does a few things. First, background, then what the heck this program is.

## Background

Ever heard of the Land effect? It's a relatively obscure optical illusion. First performed by Something Land in 19??, here are the steps:

1. Take the image, and run it through a red filter. Call this ```image_red```
2. Take the image, and run it through a green filter. Call this ```image_green```
3. Run ```image_green``` through a grayscale filter. Call this ```image_bw```
4. Project ```image_red``` and ```image_green``` on top of each other.

Instead of looking like a red filtered version of the image, you can kind of see color! There's a good demonstration [here](https://land.t-a-y-l-o-r.com/).

![Land effect image of 4999 go Momentum](https://lh3.googleusercontent.com/rd-d/ALs6j_GgZQ9G2onLaO0-vMED0H2aRlJG-WBJxqa03yOHLVHHQEwuX8u84IkKs4P6RReYA-WvxANltGSgYSpmG2oh_0dpsF1lsdmWEg2EXLgcA9444VCXYRxhEXZxXzHhT9snisqWLOuS9wXhptGQ82OmfUnYHGfSN24xeJpGAvN8v8PpqRRAe1BXZlyI0-vo6h2QMgbzPI361tPFai8QMLgQABMDU1ZbjhLLauuOL4r8A2Olf-lNDxPkNz7hMmwCfm4iCn-jRL0mEi8kw-eRKzWKHAy3hAiXIQRx01RvOe_O1DaEokXF0FXYGZCjtuzqMwFdTNdSk3L5HVYlY1SBrMOtUGMUuSUQMh38n0L7ZhiVaSPW54b_OVMOEBh-rclShn1Qpauyf6YvTfZYgmYmC4oI-b1A8ghLD_HNtuCBZgd9EGshJmWXuiDnw5OXXWCnnhZzoVXkwqHovgfncGA87m5maJb2-vfu9bO0kgnyIWtzB34D3wN18LfAWKbdBobr-USnKJI2Di4FPPEJLwk1FeE-fynizZTuXKJ3ApUZeV61smjZRS8MAUv45L3WrXL4-uD6I7UP9Ru05f7p35-9leSLrLbDOLE0my3QXB9H4MveXbAVdfiMqi07gZmWzeUO7UMvAYAlwI7RNxaAYwqeALsNyaHlZt_z-pdHKCK3sMZ_k46HHHtYoMOVK2xLYfMTdyQJMAONCDY-T7g3v2o8YvXbFPE7-aipoiSjgj6I1JsRuaR8Ptgw84cinha6YimxBEAmVyWC0WwV_pR3Mn7K9yDKYzNoVSC8e8x_OjQHgZUKwCYRvNfVWTPzBgrbBw-ezpkzPu64ugrFsKZAd_aqIwMKlqN9ymtPLfqz6sngCqCrbWbvCKGY6DntAylDAnxjI3QCmH0AkB-W28GWPhOWPLL1L8oFPYV9bqNkdOrV6a-SpBNcvyLfoKj0vXCzZwMQEg70tAAUY-JS0xDdk1HS67to2yUTHz1LxtneZiXiuqMFq-qB3Uvo2gUCDDIdze59mP9UJY0Arm0FgaSpyuGM-HOY2rmlKnfdP3EIDDZRUdQNK8imulWaiIQgCithgg_I0F1LTebvz8syI1UqSnAJ5HWhfMdOzrp5Qov2E23eUxqfPElMC9Ng0kuwjIjwQ1S1qOyymUHmL28UdWdMawDWXlcxIwQHULY97X-TUznBvPv4KoEXxhpT=w1920-h1032?auditContext=forDisplay)

Also, I learned of some experiments by Akiyoshi Kitaoka related to this effect. You can read about it [here](https://michaelbach.de/ot/col-strawbsNotRed/index.html). [Here](https://www.psy.ritsumei.ac.jp/akitaoka/ECVP2015.html) is some more of his research. He related equations describing color filtering to the Land effect. So now...

## What is this?

This is a program I wrote in C++ that does a few things. Here are the things:

- Accept an image from the user
- Simulate the image being viewed through red, green, blue, yellow, cyan, and magenta filters
- Simulate the Land effect with not just a red/green filter but every possible combination of the six previously mentioned colors
- Write the images to a folder
- Optionally open the images

It only does Windows so far. (I think?)

## How do I use it?

### Windows

Download the C++ file (or EXE). Compile the C++ file (look it up). Run the EXE.

### Linux/Mac/the others

Download the C++ file. Compile the C++ file (look it up). Run the ```.x64```?

### Haha, after I compiled it.

Ohhh...

Run it in the command line. Something like this: ```colorless_color [filename] [-o]```. The filename is the filename. Without it, a dialog box will open. The ```-o``` flag will cause the images to open as they are generated (there's 36 of them!).

By default, the images will be saved in a folder whose name matches the original image.

**Note: orange is not working properly at the moment. Any help would be appreciated.**
