Quake 2 Modeller 0.90b
---------------------

This is a Quake2 Model Editor.

This will only run on Windows 95, or Windows NT 3.51 or above.

A small Help file is included, but it is incomplete and not current to this version.
Right now, it is a little unclear, with no pictures. I intend to fix this in the future. 
Send me some comments on how you would like the help file to be, and I'll 
probably do it.

Short Feature List:
-------------------
* Import/Export MDL, MD2 files.
* Import/export .ASC .3ds .LWO files . 
* import/export skin mesh ascii files
* Copy/Pasting between models.
* Full Skin Editing (works like a mesh editor, with selecting, rotating, scaling etc). 
* Load PCX files
* Save skins to BMP format.
* Wireframe view
* Flat light sourced , and Texured view, both Z buffered

Important note: you can select multiple ascii files to import at one time. If you do, 
select them backwards. If the order doesn't look right, just play around with the 
selection order.

Also, I have enountered an error having big selections of the collection of file names 
exceeding 250 or so characters. If an error comes up in the dialog saying file not 
found, just import them in smaller groups. I have tested this sort of thing with many 
major programs, and they all seem to do the same. I will be looking into alternative 
fixes soon. 

Note About file import/exporting
--------------------------------
3D Meshes:

  MDL exporting:
  The skin mapping is NOT exported with the model. The mapping types vary quite a bit 
  from Quake2, so it was not possible to export to an mdl and retain the mapping coords.

  Ascii import:
  The ascii importer now supports multiple object ascii files. This has not been 
  extensivly tested, so if there are any bugs let me know. It also supports loading
  simple Truespace .asc files
  
  Ascii export:
  The ascii exporter exports the entire mesh as one object, with all edges visible, and 
  all faces having the same smoothing group. Has not been extensivly tested, and only 
  tested by reimporting the mesh into the Quake 2 Modeller.

  3ds import:
  Imports a 3d studio file format. Sometimes, objects are not in the correct posiiton, 
  but the vertices appear fine, and that is the most imporant bit. The cause of this 
  seems to be the heirachy of the objects. Importing of Max1.2 and Max2.0 .3ds files 
  works correctly, with the exception of incorrectly placed heirachally linked objects

  3ds export:
  Exports a 3d studio .3ds file. This correctly exports the transformation matrix so
  it loads into the correct size in Max1.2 and Max2.

  LWO import:
  Imports a lightwave Object. This has not been tested thoroughly

  LWO export:
  coming soon

  SQF import: 
  Stands for Simple Quake Frame. This is basically just a quick binary version of the
  Ascii file format. I have done a 3dsmax1.2 plugin to export this format, and will 
  release that as soon as I have a max 2 version.

2D Skin meshes:

Import and Export of the skin meshes is now available.

  Ascii export:
  Exports the skin as per a normal .asc files. All the vetices are positioned using
  the x and y co-ordnates, and the z co-ord is left at 0. The trangles are written 
  in the same order as from the Ascii exporter for the 3d meshes. Not been tested
  extensivly.

  Ascii import:
  Imports the skin mesh from a .asc file. The s and t positions are read from the 
  x and y coords respectively, and the z coord is ignored. Not been tested extensivly


Bugs
----
For this program to work correctly, run in 16-bit color mode. This will be fixed soonish.

Any other bugs, let me know. I'll fix em for ya.

For a list of stuff thats happened, read HISTORY.TXT

Info
----

This is a work in progrss, and one that I do in my spare time. I don't
regularly work on this, so no guarantee how often new version will come out.

If you have any feedback, please email me. The more you tell me, the better the 
final product will be.

Phillip Martin
martinp1@topaz.cqu.edu.au

Legal Info
----------

There are no legal restrictions on what you can do with or to this
software. I am not liable if anything goes wrong with your computer
while you're using this program. So if it does, tough luck.
