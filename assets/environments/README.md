## Blender Export Settings
To create billboards, create a plane mesh with the prefix "BB_". Ensure that the top level grouping of the mesh is named this. Ensure that you UV map the texture using the same global environment texture UV map
- ex. BB_Tree, BB_Fence_1

See EXPORT.png for export settings in Blender

After you export the file, open the .mtl file and ensure that the file path for the texture is just
- <texture>.png
- NOT ex. /path/to/somewhere/<texture>.png