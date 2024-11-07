#pragma once


namespace SimpleEngine {
	struct Material {
		float ambient_factor = { 0.1f };
		float diffuse_factor = { 1.0f };
		float specular_factor = { 0.5f };
		float shininess = { 32.f };

		float color[4] = { .0f, 1.f, 1.f, 1.f };
	};

	struct AdvanceMaterial {
		float ambient[4];
		float diffuse[4];
		float specular[4];
		float shininess;
		float color[4];
	};

	struct Gold : public AdvanceMaterial {
		float ambient[4] = { 0.2474f, 0.1995f, 0.0745f, 1.0f };
		float diffuse[4] = { 0.7516f, 0.1995f, 0.0745f, 1.0f };
		float specular[4] = { 0.2474f, 0.1995f, 0.0745f, 1.0f };
		float shininess = 51.2f;
		float color[4] = { 0, 255, 255, 255 };
	};

}
