## Import GMO Models into Blender
To import GMO models into Blender, you need to download the [Noesis Model Viewer](https://richwhitehouse.com/index.php?content=inc_projects.php&showproject=91).

Open the GMO in Noesis. You should then see the model load in with textures and animations.

![Load GMO](imgs/models/Noesis_1.png)

![Noesis viewport](imgs/models/Noesis_2.png)

Now, export the GMO model as a .fbx file, with the ```-fbxmultitake``` advanced option. (this flag allows us to export the animations individually)

![Export GMO](imgs/models/Noesis_3.png)
![Export settings](imgs/models/Noesis_4.png)

Once you have exported the .fbx, you are ready to import the model into Blender!


## Export Models from Blender
To export models from Blender, you need to install and use our custom Blender extension. This extension can be found in tools/plugins/blender & is called ```nds_model_exporter.py```.

To install a Blender plugin, go into ```Edit -> Preferences```.
![alt text](imgs/models/Blender_1.png)


Click the Intsall button, navigate to your plugin & click Install Plugin.
![alt text](imgs/models/Blender_2.png)
![alt text](imgs/models/Blender_3.png)

Once you have installed the plugin, you must ensure that the plugin is **enabled** (checkmark).
![alt text](imgs/models/Blender_4.png)

The plugin is now installed! To actually export your model, simply go to ```File -> Export -> NDS Model (.zip)```. Note that exporting may take some time.
![alt text](imgs/models/Blender_5.png)
