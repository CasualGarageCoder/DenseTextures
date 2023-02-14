#include "densetextures.h"


bool HiResTexture::_set(const StringName &p_name, const Variant &p_value) {
	if (p_name == "image") {
		if (!create_from_image(p_value)) {
			ERR_FAIL_V_MSG(false, "Can't create from image while setting the property 'image'");
		}
	} else if (p_name == "size") {
		Size2 s = p_value;
		bool recreate = (w != s.width || h != s.height);
		w = s.width;
		h = s.height;
		if (recreate) {
			create(w, h);
		}
	} else if (p_name == "_data") {
		_set_data(p_value);
	} else {
		return false;
	}

	return true;
}

bool HiResTexture::_get(const StringName &p_name, Variant &r_ret) const {
	if (p_name == "image_data") {
	} else if (p_name == "image") {
		r_ret = get_data();
	} else if (p_name == "size") {
		r_ret = Size2(w, h);
	} else {
		return false;
	}

	return true;
}

void HiResTexture::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::VECTOR2, "size", PROPERTY_HINT_NONE, ""));
}

void HiResTexture::set_data(const Ref<Image> &p_image) {
	ERR_FAIL_COND_MSG(p_image.is_null(), "Invalid image");
	bool size_cond = p_image->get_width()!=w||p_image->get_height()!=h;
	ERR_FAIL_COND_MSG(size_cond, "Invalid image size");

	VisualServer::get_singleton()->texture_set_data(texture, p_image);

	_change_notify();
	emit_changed();
}

Ref<Image> HiResTexture::get_data() const {
	return VisualServer::get_singleton()->texture_get_data(texture);
}

int HiResTexture::get_width() const {
	return w;
}

int HiResTexture::get_height() const {
	return h;
}

RID HiResTexture::get_rid() const {
	return texture;
}

bool HiResTexture::has_alpha() const {
	return true;
}

void HiResTexture::create(int p_width, int p_height) {
	ERR_FAIL_COND_MSG(p_width==0 || p_height == 0, "Can't create a texture with 0 in size coordinates"); 
	VisualServer::get_singleton()->texture_allocate(texture, p_width, p_height, 0, Image::FORMAT_RGBAF, VS::TEXTURE_TYPE_2D, 0);
	w = p_width;
	h = p_height;
	Ref<Image> img = memnew(Image(w, h, false, Image::FORMAT_RGBAF));
	VisualServer::get_singleton()->texture_set_data(texture, img);
	_change_notify();
	emit_changed();
}

bool HiResTexture::create_from_image(const Ref<Image> &img) {
	ERR_FAIL_COND_V_MSG(img.is_null(), false, "(null) reference to image");
	ERR_FAIL_COND_V_MSG(img->empty(), false, "Invalid image : empty");
	ERR_FAIL_COND_V_MSG(img->get_format()!=Image::FORMAT_RGBAF, false, "Invalid image : Not HDR");

	w = img->get_width();
	h = img->get_height();

	VisualServer::get_singleton()->texture_allocate(texture, w, h, 0, Image::FORMAT_RGBAF, VS::TEXTURE_TYPE_2D, 0); // Default Flg
	VisualServer::get_singleton()->texture_set_data(texture, img);
	_change_notify();
	emit_changed();
	return true;
}

void HiResTexture::_set_data(Dictionary p_data) {
	Ref<Image> img = p_data["image"];
	ERR_FAIL_COND(!img.is_valid());

	create_from_image(img);
};

void HiResTexture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create", "width", "height"), &HiResTexture::create);
	ClassDB::bind_method(D_METHOD("set_data", "image"), &HiResTexture::set_data);
}

HiResTexture::HiResTexture() {
	w = h = 0;
	texture = RID_PRIME(VisualServer::get_singleton()->texture_create());
	compressed = false;
}

HiResTexture::~HiResTexture() {
	VisualServer::get_singleton()->free(texture);
}

