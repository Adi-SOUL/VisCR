# VisCR - A program visualizes your continuum manipulators (1-3 sections)

## What is it?
VisCR stands for "Visualization of Continuum Manipulators", it can help you to show the motion of your continuumm manipulator while you're doing simulation stuff. You can also treat this program as a "Digital Twin" of your physical continuum manipulator.

With this program, you can:
### Showing/hiding the shape and the distal tip trajectory of CR
![Showing/hiding the shape and the distal tip trajectory of CR](https://github.com/Adi-SOUL/VisCR/blob/main/readme_img/move.gif)
![Showing/hiding the shape and the distal tip trajectory of CR](https://github.com/Adi-SOUL/VisCR/blob/main/readme_img/trajectory.gif)
### Showing/hiding the frame of the distal tip trajectory of CR
![Showing/hiding the frame of the distal tip trajectory of CR](https://github.com/Adi-SOUL/VisCR/blob/main/readme_img/frame.gif)
### Setting the amount of sections (1-3)
![Setting the amount of sections (1-3)](https://github.com/Adi-SOUL/VisCR/blob/main/readme_img/section.gif)
### Setting the amount of disks (3-10)
![Setting the amount of disks (3-10)](https://github.com/Adi-SOUL/VisCR/blob/main/readme_img/disks.gif)

This project is a subproject of [Models-in-one](https://adi-soul.github.io/Models-in-one-documentation/welcome.html) project. You can find more detials on [this page](https://adi-soul.github.io/Models-in-one-documentation/simulator.html).
![models logo.jpg](https://s2.loli.net/2023/07/25/GVKohREy58dTF3m.jpg)

## How can I use it?
### Files required:
1. Download the released program on [this page](https://github.com/Adi-SOUL/VisCR/releases);
2. Download the 'example' folder in the source code.
### Steps:
1. Setup the program. Fill the path of setup `.json` file in the `Config.ini`. The content of the `.json` file should be like this one but **without any comment**;
```json
{
	"files":{
		"arrow": { // arrow model for coordinate system and tip force
			"file_path": "D:/Documents/VisCR/STL_files/arrow.STL",
			"size": {
				"x": "1.f",  // size in mm
				"y": "1.f",
				"z": "13.f"
			}
		},
		"base": { 
			"file_path": "D:/Documents/VisCR/STL_files/base_disk.STL",
			"size": {
				"x": "24.f",
				"y": "24.f",
				"z": "12.f"
			}
		},
		"section_1": { // model for disks
			"file_path": "D:/Documents/VisCR/STL_files/mid_disk_1.STL",
			"size": {
				"x": "24.f",
				"y": "24.f",
				"z": "1.8f"
			}
		},
		"section_2": {
			"file_path": "D:/Documents/VisCR/STL_files/mid_disk_2.STL",
			"size": {
				"x": "24.f",
				"y": "24.f",
				"z": "1.8f"
			}
		},
		"tendon": {
			"file_path": "D:/Documents/VisCR/STL_files/tendon.STL",
			"size": {
				"x": ".5f",
				"y": ".5f",
				"z": "3.4f"
			}
		},
		"tip": {
			"file_path": "D:/Documents/VisCR/STL_files/tip_disk.STL",
			"size": {
				"x": "24.f",
				"y": "24.f",
				"z": "12.f"
			}
		}
	},
	"parameters":{
		"total_length": "324.f", // also in mm
		"section_number": "2",
		"disk_number_section_1": "6", // disk number of each section 
		"disk_number_section_2": "6" // -1 if section 2 does not exist
	}
}

```

2. Realize your own `default_one_step` function in `main.py`;

**NOTE:** This function accepts an int type variable to identify the subscript of the current simulation step and returns an `ndarray` list. The physical meaning of this function is: Returns the homogeneous transformation matrix of **each point** of the continuum robot starting from the start point during the `Step: int` running step. For example:
```python
import numpy
def sample_func(step: int) -> list[numpy.ndarray]:
	res: list[numpy.ndarray] = []
	L: float = 200/40.
	theta: float = numpy.pi/3
	phi: float = numpy.pi*2/100*step
	for i in range(100):
		# get_T is a function that calcuates the homogeneous transformation matrix.
		res.append(get_T(theta/100*(i+1), phi, L/100*(i+1)))  
	return res
```

3. Start the VisCR program, click "start" at its bottom;
4. Run `main.py`

That is all you need to do!

Or you can also read [this file](https://github.com/Adi-SOUL/VisCR/blob/main/connect.cpp) and realize your own code.

## Spical Thanks
[The Qt Project](https://www.qt.io/)
