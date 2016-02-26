#ifndef __mesh_slicer_info__
#define __mesh_slicer_info__

struct MeshSlicerInfo {
	// smoothing angle to apply to meshes
	double smoothingAngle;

	MeshSlicerInfo() {
		smoothingAngle = 30.0;
	}
};

#endif /* __mesh_slicer_info__ */