#ifndef DENSE_TEXTURES_H
#define DENSE_TEXTURES_H

#include "scene/resources/texture.h"

class HiResTexture : public Texture {
	GDCLASS(HiResTexture, Texture);

	public:

	private:
	RID texture;
	int w, h;
	bool compressed;

	protected:

	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;

	static void _bind_methods();

	void _set_data(Dictionary p_data);

	bool create_from_image(const Ref<Image> &p_image);

	public:
	void create(int p_width, int p_height);

	void set_data(const Ref<Image> &p_image);
	Ref<Image> get_data() const;

	void set_flags(uint32_t p_flags) {}
	uint32_t get_flags() const { return 0; }

	bool has_alpha() const;

	int get_width() const;
	int get_height() const;

	virtual RID get_rid() const;

	HiResTexture();
	~HiResTexture();
};

#endif // DENSE_TEXTURES_H
