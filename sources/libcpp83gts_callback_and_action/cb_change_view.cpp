#include <FL/fl_ask.H> // fl_alert()
#include "ptbl_returncode.h"
#include "pri.h"
#include "gts_gui.h"
#include "gts_master.h"

void gts_master::cb_real_time_on_off( void )
{
	/* データがないときはなにもしない */
	if (NULL == this->cl_iip_ro90.get_clp_parent()) {
		return;
	}

	/* データが、RGBフルカラーでないと実行しない */
	if (this->cl_iip_ro90.get_l_channels() < 3L) {
		return;
	}

	/* メニュー選択が、RGBフルカラーでないと実行しない */
	switch (cl_gts_gui.choice_pixel_type->value()) {
	case 0:
	case 1:
		return;
	}

	/* トレス準備をまだやっていないときは実行 */
	if (this->cl_iip_trac.get_l_channels() < 3L) {
		if (OK != this->_iipg_color_trace_setup()) {
			pri_funct_err_bttvr(
		 "Error : this->_iipg_color_trace_setup() returns NG" );
			return;
		}
	}

	/* 表示準備 */
	/***if (OK != this->_iipg_view_setup()) {
		pri_funct_err_bttvr(
	 "Error : this->_iipg_view_setup() returns NG" );
		return;
	}***/

	/* 分割画面でなければなにもしない */
	switch (this->cl_ogl_view.get_e_wview_type()) {
	case E_WVIEW_TYPE_NOTHING: break;
	case E_WVIEW_TYPE_MAIN: break;
	case E_WVIEW_TYPE_SUB: this->iipg_view_redraw_(); break;
	case E_WVIEW_TYPE_LR_PARALLEL: this->iipg_view_redraw_(); break;
	case E_WVIEW_TYPE_UD_PARALLEL: this->iipg_view_redraw_(); break;
	case E_WVIEW_TYPE_LR_ONION: break;
	case E_WVIEW_TYPE_UD_ONION: break;
	}
}

void gts_master::cb_change_wview_( E_WVIEW_TYPE wview_type )
{
	/* 画面の表示変更 */
	switch (wview_type) {
	case E_WVIEW_TYPE_NOTHING: break;
	case E_WVIEW_TYPE_MAIN:		this->_wview_main(); break;
	case E_WVIEW_TYPE_SUB:		this->_wview_sub();  break;
	case E_WVIEW_TYPE_LR_PARALLEL:	this->_wview_lr_parallel(); break;
	case E_WVIEW_TYPE_UD_PARALLEL:	this->_wview_ud_parallel(); break;
	case E_WVIEW_TYPE_LR_ONION: break;
	case E_WVIEW_TYPE_UD_ONION: break;
	}

	/* 表示 */
	this->iipg_view_redraw_();
}

bool gts_master::check_change_wview( void )
{
	/* データがないときはなにもしない */
	if (NULL == this->cl_iip_ro90.get_clp_parent()) {
		fl_alert("No Image");
		return false;
	}

	/* データが、RGBフルカラーでないと実行しない */
	if (this->cl_iip_ro90.get_l_channels() < 3L) {
		fl_alert("Not RGB Image");
		return false;
	}

	/* 表示準備 */
	if (OK != this->_iipg_view_setup()) {
		fl_alert("View setup Error");
		pri_funct_err_bttvr(
	 "Error : this->_iipg_view_setup() returns NG" );
		return false;
	}
	return true;
}
void gts_master::cb_change_wview_main( void )
{ this->cb_change_wview_( E_WVIEW_TYPE_MAIN );
}
void gts_master::cb_change_wview_sub( void )
{ this->cb_change_wview_( E_WVIEW_TYPE_SUB );
}
void gts_master::cb_change_wview_lr( void )
{ this->cb_change_wview_( E_WVIEW_TYPE_LR_PARALLEL );
}
void gts_master::cb_change_wview_ud( void )
{ this->cb_change_wview_( E_WVIEW_TYPE_UD_PARALLEL );
}

