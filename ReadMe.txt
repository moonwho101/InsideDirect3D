
                        Companion CD README
                                for
                          Inside Direct3D
  
                          by Peter Kovach
  
              Copyright (c) 2000 Microsoft Corporation.
                        All Rights Reserved.
________________________________________________________________________

README Contents
 - Special Note
 - Acknowledgments
 - What's on the CD?
 - Support Information
 - Afterword

========================================================================
Special Note
========================================================================
To compile and run the RoadRage sample code, you first must install  
the DirectX 7 SDK. To install the DirectX 7 SDK on your computer, run
d:\Dx7SDK\Install.exe (where d: is your CD-ROM drive) and follow the
instructions.

========================================================================
Acknowledgments
========================================================================
I'd again like to thank the individuals and companies who have  
permitted me to use their material on this CD:

 - 3DMark(tm)2000 used by permission of Nathan Harley at MadOnion.com.
 - HyperSnap-DX used by permission of Greg Kochaniak at Hyperionics.
 - LifeForms 3 used by permission of Credo Interactive Inc.
 - PolyTrans used by permission of Robert Lansdale at Okino Computer
   Graphics, Inc. The PolyTrans Model Translator is Copyright c 1988-1999
   Okino Computer Graphics, Inc.
 - Quake 2 Modeller used by permission of Phillip Martin.
 - RenderIt 3D! and Materialize 3D! used by permission of Steve Eschweiler
   at Indotek Software Productions.
 - Portions of the RoadRage code, includingZebra. ppm / .bmp, 
   Door3.ppm / .bmp, Grass3.ppm / .bmp, Island.ppm / .bmp, 
   Lampave2.ppm / .bmp, Petrol.ppm / .bmp, Petrol2.ppm / .bmp,
   Road1.ppm / .bmp, Road2.ppm / .bmp, traffic.ppm / .bmp, car1.ppm / .bmp,
   window.ppm / .bmp, window2.ppm / .bmp, Lrskin5.ppm / .bmp, 
   Stonewal.ppm / .bmp, Mmchrome.ppm / .bmp, Pave1.ppm / .bmp,
   Redbrick.ppm / .bmp (the original brick pattern is from Axem Textures),
   all road sections in  .3ds / .dat, lamp_post.dat, petrol_station.dat,
   and jeep2.3ds, used by permission of William Chin.
 - Portions of the RoadRage code (Semi.3ds / .dat, Semileft.3ds / .dat,
   Semiright.3ds / .dat, Semileft2.3ds / .dat, Semiright2.3ds / .dat,
   wh2.3ds / .dat, mbpetrol.3ds / .dat, mbpetrol2.3ds / .dat, 
   mbshop.3ds / .dat, mbshop2.3ds / .dat, mbparade.3ds / .dat,
   mbparade2.3ds / .dat, and mbparade3.3ds / .dat) used by permission
   of Mark Bracey. (Note: I converted some of these models to RR .dat
   format and included them with my own models in "objects.dat".)
 - Code for many animated models in RoadRage (Heli.md2, HiVelocity.md2,
   NeoBike.md2, Bdozer.md2, Cronex.md2, Malignus.md2, Robot.md2, Flamer,
   Shotgun, plus all pcx, ppm, bmp, and psp skins of the same name) used
   by permission of Blair Harris Burrill, who can be contacted at 
   Greigus@Aol.com or at his Web site at www.BusyBurrills.com/neoXS. 
 - The textures flrtiles.bmp, planks.bmp, pave.bmp, plastic1.bmp, 
   contan.bmp, and mahog.bmp are used by permission of Correia Emmanuel. 


========================================================================
What's on the CD?
========================================================================

3DMark2K Directory -- 3DMark(tm)2000
---------------------------------
3DMark(tm)2000 - The Gamers' Benchmark, produced by MadOnion.com, is the 
first 3D benchmark aimed directly at the 3D gaming community. It is 
designed for benchmarking the new generation of 3D hardware accelerators 
and uses DirectX 7. 3DMark(tm)2000 is an excellent benchmarking package 
that lets you determine the capabilities of your 3D accelerator hardware.
In addition, the quality of this software provides an ideal you should 
strive for in your software development efforts.


DX7SDK Directory
----------------
The DirectX 7 Software Development Kit.


EBook Directory
---------------
The electronic version of Inside Direct3D.


HypSnap Directory -- HyperSnap-DX
---------------------------------
This tool, provided by Greg Kochaniak of Hyperionics, is the best screen 
grab tool I've found. It is provided as shareware and certainly worth 
its low price. I used this tool to grab all the screen shots in this 
book. This tool lets you grab a screen shot of a Direct3D application 
running in full-screen mode--a unique capability. Most screen grab 
utilities fail when attempting to handle full-screen DirectX screen 
shots, but this product performs wonderfully. The demo version of this 
program is included on this CD. It is a fully functional version of the 
application, but each screen capture image includes an indication that 
the screen shot was acquired using HyperSnap-DX. You can find upgrade 
information from the Hyperionics Web site at www.hyperionics.com.


LifeFrms Directory -- LifeForms 3 and Animated Character Models
---------------------------------------------------------------
This software is available from Credo Interactive. LifeForms 3 is an 
excellent character animation package that lets you easily animate 3D 
characters. Although animating characters is usually one of the most 
difficult tasks in 3D game development, LifeForms 3 makes this process 
relatively easy.

The trial version of LifeForms 3 requires a serial number, which you can
obtain from Credo Software by sending email to

    sales@CharacterMotion.com
    Subject: Request key for Inside Direct3D trial version

or by telephoning Credo Interactive Inc. at

    1-888-291-7453 or 604-291-6717


PolyTrns Directory -- PolyTrans
-------------------------------
This program, provided by Robert Lansdale of Okino Computer Graphics, Inc., 
is by far the best 3D model conversion tool on the market. In addition, it's 
very intuitive and thus quite easy to use. It allows you to import files 
in a huge array of formats, including 3D Studio, Imagine, LightWave, 
TrueSpace, WaveFront, and basically all the other popular formats. This 
program flawlessly handles all the nuances of the various file formats 
and supports conversion of animation data, which is something that often
trips up other conversion packages.

Note: If you have trouble installing this program, contact Okino Computer
Graphics, Inc., at

    http://www.okino.com


Q2Mod -- Quake 2 Modeller
-------------------------
Phillip Martin wrote this program. Even though this program is freeware, 
it's a fully functional, professional piece of software. Models created 
with the Quake 2 Modeller contain a series of frame sequences that 
represent the following motions:

* Stand    0 - 39
* Run     40 - 45
* Attack  46 - 53
* Pain 1  54 - 57
* Pain 2  58 - 61
* Pain 3  62 - 65
* Jump    66 - 71
* Flip    72 - 83
* Salute  84 - 94
* Taunt   95 - 111
* Wave   112 - 122
* Point  123 - 134

Each of these motion sequences can be triggered using RoadRage. Take a 
look at the CLoadWorld::LoadPlayerAnimationSequenceList routine in 
loadworld.cpp. This code specifies the beginning and ending frame for 
each of these sequences. The keyboard, mouse, or joystick handler 
routines use these sequences. When the user presses a key, the associated
motion sequence is played. As an example, when the user presses the 
h key, the salute sequence is played--that is, a series of animated frames
is rendered so that the character moves to perform the selected motion.
The Quake 2 Modeller also provides the ability to load the skins for the
model. The skin is the texture applied to the model to make it look 
realistic. Using this tool, you can create new models to use in the 
RoadRage code. You can load and animate each model by using the code in 
Chapter 14. You can also find thousands of free models in the Quake II 
model format on the Internet. You'll probably be able to find a character
that fits your needs or that you can modify slightly by using this tool 
(by changing the 3D points).  Keep in mind that you can always edit the 
textures to change the appearance. 


RenderIt Directory -- RenderIt 3D! and Materialize 3D!
-----------------------------------------------------
RenderIt 3D! is a commercial product produced by Indotek that supplies 
a wrapper for Direct3D Immediate Mode. It provides a handy set of 
commands that support an unlimited number of models and surfaces, 
true-color mode support, texture animation support, AGP support, 
windowed mode support, mipmapping, virtual surfaces, hardware-accelerated
DirectDraw surfaces, anti-aliased image scaling, source code for a level
editor that works with this package, and a variety of other capabilities.
Using just six function calls, you can set up DirectDraw and Direct3D and
render a 3D model:

r3d_Enumerate
r3d_Initialize
r3d_ListAdd3DModel
r3d_ListLoad
r3d_RenderBegin
r3d_Render3DModel

In addition to these commands, there are numerous commands in the 
following categories. (Each category contains between 2 and 20.)

* 2D rendering functions
* DirectDraw surface functions
* Virtual surface functions
* Texture surface functions
* File access functions
* Timing functions
* Windowed-mode functions
* DirectX interface functions
* Matrix manipulation functions
* Model, material, and texture management functions
* 3D model manipulation functions
* 3D rendering functions
* Miscellaneous functions

The level editor program can import 3D Studio models (.3ds format) along 
with their textures.  You can then just add code to export the levels in
the format you choose for your game.

The setup for RenderIt 3D! also installs an evaluation copy of 
Materialize 3D!. Materialize 3D! is a 3D model texture editor, converter,
and polygon processor that you can use to create and refine textures 
for 3D objects. You can find all the information you need on RenderIt 3D!
and Materialize 3D! at the Indotek Web site, www.indotek.com. The 
current versions of these programs use DirectX 6 rather than DirectX 7, 
but I'm sure upgraded versions will be available soon: watch their Web 
site.
  

RoadRage Directory -- Sample code for Inside Direct3D
-----------------------------------------------------
Starting with the Chap2 subproject, which contains the code for 
Chapter 2, the subprojects that are contained on this CD build on one 
another. Here is a list of all the subprojects:

.\Chap2 
    This subproject creates the Microsoft Windows framework for the 
    RoadRage application, including the menus used to control the 
    rendering parameters. 

.\Chap3
    This subproject adds the DirectDraw code for the RoadRage program 
    to the Chap2 subproject.

.\Chap4
    This subproject adds the Direct3D code necessary to create all the 
    main Direct3D objects: the Direct3D, Direct3DDevice, and z-buffer.  
    As with all code I produce, I assume that the newest version of 
    DirectX is running rather than check for which version exists. 
    Because I would ship the newest version of DirectX with any 
    application, I could be sure the most current version of DirectX 7 
    would be installed automatically if necessary.

.\Chap5
    This subproject adds the code for handling transforms, lighting, 
    and vertices. The code creates the world, view, and projection 
    matrices. It then handles transforming these matrices, handling 
    rotations, translations, and scaling. A variety of functions to 
    handle all necessary aspects of matrix transformations are provided. 
    The code to generate the first 3D object is then added, using the 
    vertices introduced in this chapter. Finally, the code to produce 
    all the light types is added, and the code cycles through the light 
    types, showing the effect of each light type on a simple scene 
    composed of a few rectangular wall segments.

.\Chap6
    This subproject adds all the vertex formats available to render the 
    objects. Each vertex format is then used with DrawPrimitive code 
    to produce the final 3D objects. The code to generate many of the 
    DrawPrimitive types, such as triangle strips and triangle fans, is 
    added. The data for these objects is loaded from data files--the 
    code to load the data is integrated. Outline and fill states are 
    also used to set up the code to render wire-frame objects. Wire 
    frame is used because at this point no textures have been added and 
    it's easier to see the 3D content as wire frame when no textures are 
    displayed.  

.\Chap7
    This subproject adds support for keyboard, mouse, and joystick 
    control of motion through the 3D world. This control is added using 
    DirectInput. The code supports enumerating all device types, 
    including force-feedback devices. The code produces forces to 
    illustrate the use of force feedback to add additional realism to 
    3D games. A new menu item is added, with submenus allowing the user 
    to select the keyboard, mouse, or any connected joysticks (located 
    during enumeration) as the primary input device.

.\Chap8
    This subproject demonstrates how to create and apply textures to a 
    3D world.

.\Chap9
    This subproject shows how to add fog to a scene. A menu item that 
    allows the user to select whether fog is to be used is added.

.\Chap10
    This subproject adds support of alpha blending to the code. Alpha 
    blending allows you to let users see through windows of the 
    buildings.

.\Chap13
    This subproject adds animated characters to the 3D world. Quake 
    II model format characters are added to the world and then animated.
    As you walk through the world, you'll see the characters moving.  

.\Chap15
    The code added to this chapter integrates head-to-head play using 
    DirectPlay. Other players will now be able to control the animated 
    models added in the Chap13 subproject. The code locates existing 
    game sessions or initiates a new one and provides connection 
    capabilities via  a modem, a direct cable connection, or the Internet.

.\Final
    This is the final project and contains everything covered in the book.  
    This program supports DirectInput, DirectPlay, DirectSound, 3D 
    animated characters, and so on. If you want to see something 
    DirectX provides, check the code. Using this project, you should 
    have a superb basis for any new 3D game or program you want to create.

.\RR_Editor
    This project supplies a program developed by William Chin. The 
    program allows you to generate a 3D world that the RoadRage code 
    can use. This graphics utility is very easy to use. It  allows you 
    to select road segments, buildings, street lights, and so on and to 
    place them on the world map using drag-and-drop. This program outputs
    the files in the level1.map file. Each item along the right side of 
    the screen represents a road segment, such as a curve or a four-way 
    intersection. As you select a road segment, it is added to the 
    currently selected segment so that you can easily add one segment 
    after another. With this editor, you can build up an infinite variety
    of landscapes. You can also create new textures for each object so 
    that you can change its appearance without having to change the 
    underlying 3D model.


========================================================================
Support Information
========================================================================

Microsoft Press support information
-----------------------------------

Every effort has been made to ensure the accuracy of the book
and the contents of this companion CD. Microsoft Press
provides corrections for books through the World Wide Web at

     http://mspress.microsoft.com/support/

If you have comments, questions, or ideas regarding the book or
this companion disc, please send them to Microsoft Press via 
e-mail to:

    MSPINPUT@MICROSOFT.COM

or via postal mail to:

    Microsoft Press
    Attn:  Inside Direct3D Editor
    One Microsoft Way
    Redmond, WA  98052-6399

Please note that product support is not offered through the
above addresses.


========================================================================
Afterword
========================================================================
Using all the code on this CD along with the numerous high-quality tools, 
you should be able to create any 3D program you can imagine. As you 
develop your programs, models, or any other interesting 3D related entity,
feel free to e-mail me with details of your work or even the product 
itself if you're interested in posting it at the freeware portion of 
Head Tapper Software's DirectX site for use by others in their efforts 
to gain expertise in using Direct3D.

Peter Kovach
peter@headtappersoftware.com