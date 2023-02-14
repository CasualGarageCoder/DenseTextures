#ifndef DENSE_TEXTURES_H
#define DENSE_TEXTURES_H

#include "scene/resources/texture.h"

#include "core/core_string_names.h"

class HiResTexture : public Texture {
	GDCLASS(HiResTexture, Texture);

	public:

	private:
	RID texture;
	int w, h;

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

class DenseGradientTexture : public Texture {
	GDCLASS(DenseGradientTexture, Texture);

	public:
	struct Point {
		float offset;
		Color color;
		bool operator<(const Point &p_ponit) const {
			return offset < p_ponit.offset;
		}
	};

	private:
	Ref<Gradient> gradient;
	RID texture;
	int width;
	bool texture_reset;

	void _update();

	protected:
	static void _bind_methods();

	public:
	void set_gradient(Ref<Gradient> p_gradient);
	Ref<Gradient> get_gradient() const;

	void update_gradient_only(Ref<Gradient> p_gradient);

	void set_width(int p_width);
	int get_width() const;

	virtual RID get_rid() const { return texture; }
	virtual int get_height() const { return 1; }
	virtual bool has_alpha() const { return true; }

	virtual void set_flags(uint32_t p_flags) {}
	virtual uint32_t get_flags() const { return FLAG_FILTER; }

	virtual Ref<Image> get_data() const;

	DenseGradientTexture();
	virtual ~DenseGradientTexture();
};

#endif // DENSE_TEXTURES_H
