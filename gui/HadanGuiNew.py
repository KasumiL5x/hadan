#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from PySide import QtCore, QtGui
from shiboken import wrapInstance
from maya import OpenMayaUI as omui
import maya.cmds as cmds
import maya.mel as mel
import os

maya_main_window_ptr = omui.MQtUtil().mainWindow()
maya_main_window = wrapInstance(long(maya_main_window_ptr), QtGui.QWidget)

class HadanHowTo(QtGui.QWidget):
	def __init__(self):
		QtGui.QWidget.__init__(self)
		self.__build_ui()

	def __build_ui(self):
		self.setGeometry(QtCore.QRect(100, 100, 450, 500))
		self.setWindowTitle(QtGui.QApplication.translate("Hadan", "破断", None, QtGui.QApplication.UnicodeUTF8))

		title_lbl = QtGui.QLabel(self)
		title_lbl.setGeometry(QtCore.QRect(10, 0, 75, 50))
		font = QtGui.QFont()
		font.setPointSize(16)
		title_lbl.setFont(font)
		title_lbl.setText("Help")

		split_options = QtGui.QFrame(self)
		split_options.setGeometry(QtCore.QRect(5, 40, 440, 16))
		split_options.setFrameShape(QtGui.QFrame.HLine)
		split_options.setFrameShadow(QtGui.QFrame.Sunken)

		hadan_str = 'Hadan is a multi-purpose fracturing tool.<br/><br/>'
		usage_str = ('<b>Usage</b><br/>Select an object, configure the tool, click <i>Fracture</i>! ' +
			'It is <b>very important</b> to check the <b>Output Window</b> for program status output. ' +
			'<i>Gap</i> can be used to separate chunks by moving their vertices along their normals.<br/><br/>')
		uniform_str = ('<b>Uniform</b><br/>' +
			'Even randomly distributed points within the object\'s bounding box.<br/>' +
			'<i>Uniform</i> points are randomly generated within the object\'s bounding box.<br/><br/>')
		cluster_str = ('<b>Cluster</b><br/>' +
			'Points source a cluster of surrounding points, joined by some uniform points.<br/>' +
			'<i>Positions</i> are appended by <i>Primary</i> randomly generated points.  Each point is then ' +
			'used to randomly generate <i>Secondary</i> points around it, within a distance of <i>Flux</i> percent ' +
			'of the size of the object\'s bounding box.  <i>Uniform</i> randomly generated points are also added.<br/><br/>')
		bezier_str = ('<b>Bezier</b><br/>' +
			'Simulates a propagating crack using a bezier curve and uniform points.<br/>' +
			'The process differs based on the number of <i>Positions</i>.  If none are provided, all four points of the curve ' +
			'are randomly generated; the first and last are on the surface of the object, and will attempt to be placed at least ' +
			'<i>Min Bezier Dist</i> percent of the bounding box\'s size apart (although if a maximum iteration count is hit, the last attempt ' +
			' will be taken regardless of the distance), and the two intermediate points are randomly generated within the object\'s ' +
			'bounding box.  If two are provided, they are assumed to be the beginning and end points, respectively; the two intermediate ' +
			'points will be generated randomly.  If all four are given, the curve is assumed to follow those points\' path.  ' +
			'The bezier curve is sampled to generate <i>Samples</i> points along the curve.  These sampled points are moved ' +
			'randomly away from the curve in the range [<i>-Flux</i>, <i>Flux</i>] percent of the bounding box\'s size. ' +
			'<i>Uniform</i> randomly generated points are also added for variation.<br/><br/>')
		ui_str = ('The <i>Settings</i> tab is where the majority of configurable options can be tweaked.  The <i>Positions</i> ' +
			'tab is where scene objects can be managed as sources.  For instance, as input for the <i>bezier</i> fracture mode.  Note that ' +
			'object names are stored and are evaluated for positions only upon executing the fracture itself, so they can be added at any time. ' +
			'The <i>Advanced</i> tab shows extra information, like the previously generated command.')
		tb = QtGui.QTextEdit(self)
		tb.setGeometry(QtCore.QRect(5, 60, 440, 400))
		tb.setReadOnly(True)
		tb.setText(hadan_str + usage_str + uniform_str + cluster_str + bezier_str + ui_str)
	#end
#end

class HadanGui(QtGui.QMainWindow):
	def __init__(self):
		super(HadanGui, self).__init__()
		self.__load_plugin()
		self.__build_ui()
		self.__command = ''
		self.__positions = []
	#end

	def __rebuild_command(self):
		selected_mesh = self.__get_selected_mesh()
		if None == selected_mesh:
			cmds.warning('Please select a mesh.')
			self.txt_generatedCommand.setText("syntax error");
			self.__command = ''
			return

		# command name
		self.__command = 'hadan '

		# object to fracture
		self.__command += '-mn %s ' % str(selected_mesh)

		# build based on fracture type
		ft = self.__get_fracture_type()
		if 'uniform' == ft:
			self.__command += '-ft uniform '
			self.__command += '-uc %d ' % (self.spin_uniform.value())
			self.__command += ('-sd %f ' % (self.spin_gap.value())) if self.spin_gap.value() != 0.0 else ''
		elif 'cluster' == ft:
			self.__command += '-ft cluster '
			self.__command += '-uc %d ' % (self.spin_uniform.value())
			self.__command += '-pc %d ' % (self.spin_primary.value())
			self.__command += '-sc %d ' % (self.spin_secondary.value())
			self.__command += '-flp %f ' % (self.spin_flux.value())
			self.__command += ('-sd %f ' % (self.spin_gap.value())) if self.spin_gap.value() != 0.0 else ''
		elif 'bezier' == ft:
			self.__command += '-ft bezier '
			self.__command += '-uc %d ' % (self.spin_uniform.value())
			self.__command += '-sam %d ' % (self.spin_samples.value())
			self.__command += '-flp %f ' % (self.spin_flux.value())
			self.__command += ('-sd %f ' % (self.spin_gap.value())) if self.spin_gap.value() != 0.0 else ''

		# build based on slicer type
		st = self.__get_slicer_type()
		if 'gte' == st:
			self.__command += '-st GTE '
		elif 'csgjs' == st:
			self.__command += '-st CSGJS '

		# seed value
		self.__command += '-rs %d ' % (self.spin_seed.value())

		# smoothing angle
		self.__command += '-sa %f ' % (self.spin_smoothing.value())

		# min bezier distance
		self.__command += '-mbd %f ' % (self.spin_min_bezier_dist.value())

		# multi-threading
		self.__command += '-mt %s' % ('true' if self.chk_multithreaded.isChecked() else 'false')

		# add all user points
		for curr in self.__positions:
			if not cmds.objExists(curr):
				continue
			pos = cmds.xform(curr, q=True, ws=True, t=True)
			self.__command += '-pnt %f %f %f ' % (pos[0], pos[1], pos[2])
		#end

		# strip trailing whitespace
		self.__command = self.__command.rstrip()

		# update generated command textarea
		self.txt_generatedCommand.setText(self.__command)
	#end

	def __get_selected_mesh(self):
		sl = cmds.ls(sl=True, type='transform')
		if 1 != len(sl):
			return None
		for curr in cmds.listRelatives(sl):
			if 'mesh' == cmds.nodeType(curr):
				return sl[0]
		return None
	#end

	def __rebuild_positions_list(self):
		self.lv_positions.clear()
		for curr in self.__positions:
			self.lv_positions.addItem(curr)
	#end

	def __fracture_cb(self):
		self.__rebuild_command()
		print 'Running command: ' + self.__command
		mel.eval(self.__command)
	#end

	def __move_position_up(self):
		curr_idx = self.lv_positions.currentIndex().row()
		# cannot move top element or no selection
		if curr_idx <= 0:
			return
		self.__positions[curr_idx], self.__positions[curr_idx-1] = self.__positions[curr_idx-1], self.__positions[curr_idx]
		self.__rebuild_positions_list()
		self.lv_positions.setCurrentRow(curr_idx-1)
	#end

	def __move_position_down(self):
		curr_idx = self.lv_positions.currentIndex().row()
		if -1 == curr_idx or curr_idx >= self.lv_positions.count()-1:
			return
		self.__positions[curr_idx], self.__positions[curr_idx+1] = self.__positions[curr_idx+1], self.__positions[curr_idx]
		self.__rebuild_positions_list()
		self.lv_positions.setCurrentRow(curr_idx+1)
	#end

	def __add_selected_cb(self):
		for curr in cmds.ls(sl=True, type='transform'):
			if curr in self.__positions:
				continue
			self.__positions.append(curr)
		#end
		self.__rebuild_positions_list()
	#end

	def __remove_cb(self):
		item = self.lv_positions.currentItem()
		if None is item:
			return
		del self.__positions[self.__positions.index(item.text())]
		self.__rebuild_positions_list()
	#end

	def __clear_cb(self):
		self.__positions = []
		self.__rebuild_positions_list()
	#end

	def __how_to_use_cb(self):
		main_wnd_pos = self.pos()

		self.help_wnd = HadanHowTo()
		self.help_wnd.setWindowModality(QtCore.Qt.ApplicationModal)
		self.help_wnd.move(main_wnd_pos)
		self.help_wnd.show()
	#end

	def __get_fracture_type(self):
		return self.cb_fractureType.currentText().lower()
	#end

	def __get_slicer_type(self):
		return self.cb_slicer.currentText().lower()
	#end

	def __get_hadan_filename(self):
		# search all plugin directories for the hadan file
		for curr_path in os.environ['MAYA_PLUG_IN_PATH'].split(';'):
			# append trailing /
			curr_dir = (curr_path + '/') if curr_path[-1] != '/' else (curr_path)
			# check for hadan-d.mll
			if os.path.isfile(curr_dir + 'hadan-d.mll'):
				return 'hadan-d.mll'
			elif os.path.isfile(curr_dir + 'hadan.mll'):
				return 'hadan.mll'
		#end
		return None
	#end

	def __load_plugin(self):
		hadan_file_name = self.__get_hadan_filename()
		# make sure to fail if the file doesn't exist
		if None == hadan_file_name:
			cmds.warning('Could not find hadan plugin.')

		# load found plugin
		if not cmds.pluginInfo(hadan_file_name, l=True, q=True):
			if None == cmds.loadPlugin(hadan_file_name, qt=True):
				cmds.warning('Failed to load hadan plugin.')
				return False
			cmds.warning('Successfully loaded plugin: ' + hadan_file_name)
		else:
			cmds.warning('Skipped loading plugin: ' + hadan_file_name)
		return True
	#end

	def __unload_plugin(self):
		hadan_file_name = self.__get_hadan_filename()
		# make sure to fail if the file doesn't exist
		if None == hadan_file_name:
			cmds.warning('Could not find hadan plugin.')

		# unload found plugin
		if cmds.pluginInfo(hadan_file_name, l=True, q=True):
			if None == cmds.unloadPlugin(hadan_file_name, f=True):
				cmds.warning('Failed to unload hadan plugin.')
				return False
			cmds.warning('Successfully unloaded plugin: ' + hadan_file_name)
		else:
			cmds.warning('Skipped unloading plugin: ' + hadan_file_name)
		return True
	#end

	def closeEvent(self, event):
		self.__unload_plugin()
		event.accept() # let the window close
	#end

	def __build_ui(self):
		# set parent window to maya's window
		self.setParent(maya_main_window)

		# ensure type of window
		self.setWindowFlags(QtCore.Qt.Window)

		# create common fonts
		font_10 = QtGui.QFont()
		font_10.setPointSize(10)
		font_12 = QtGui.QFont()
		font_12.setPointSize(12)

		# main window
		self.setObjectName("self")
		self.resize(400, 307)
		self.setWindowTitle(QtGui.QApplication.translate("Hadan", "破断", None, QtGui.QApplication.UnicodeUTF8))

		# logo label
		self.lbl_logo = QtGui.QLabel(self)
		self.lbl_logo.setGeometry(QtCore.QRect(10, 2, 240, 50))
		font = QtGui.QFont()
		font.setPointSize(28)
		self.lbl_logo.setFont(font)
		self.lbl_logo.setObjectName("lbl_logo")
		self.lbl_logo.setText(QtGui.QApplication.translate("Hadan", "破断", None, QtGui.QApplication.UnicodeUTF8))

		# logo image
		# pixmap = QtGui.QPixmap('D:/uni/ma_vfx/fx_anim/hadan/gui/fracture.png')
		# scaled_pixmap = pixmap.scaled(self.lbl_logo.size(), QtCore.Qt.KeepAspectRatio)
		# self.lbl_logo.setPixmap(scaled_pixmap)

		# how to use button
		self.btn_howToUse = QtGui.QPushButton(self)
		self.btn_howToUse.setGeometry(QtCore.QRect(315, 22, 75, 23))
		self.btn_howToUse.setObjectName("btn_howToUse")
		self.btn_howToUse.setText(QtGui.QApplication.translate("How To Use?", "How To Use?", None, QtGui.QApplication.UnicodeUTF8))
		self.btn_howToUse.clicked.connect(self.__how_to_use_cb)

		# main tab area
		self.ta_main = QtGui.QTabWidget(self)
		self.ta_main.setGeometry(QtCore.QRect(10, 60, 381, 231))
		self.ta_main.setObjectName("ta_main")
		self.ta_main.setCurrentIndex(0)

		# settings tab page
		self.tp_settings = QtGui.QWidget()
		self.tp_settings.setObjectName("tp_settings")
		self.ta_main.addTab(self.tp_settings, "")
		self.ta_main.setTabText(self.ta_main.indexOf(self.tp_settings), QtGui.QApplication.translate("Settings", "Settings", None, QtGui.QApplication.UnicodeUTF8))

		# fracture type label
		self.lbl_fractureType = QtGui.QLabel(self.tp_settings)
		self.lbl_fractureType.setGeometry(QtCore.QRect(10, 10, 79, 16))
		self.lbl_fractureType.setFont(font_10)
		self.lbl_fractureType.setObjectName("lbl_fractureType")
		self.lbl_fractureType.setText(QtGui.QApplication.translate("Fracture Type", "Fracture Type", None, QtGui.QApplication.UnicodeUTF8))

		# fracture type combo box
		self.cb_fractureType = QtGui.QComboBox(self.tp_settings)
		self.cb_fractureType.setGeometry(QtCore.QRect(95, 7, 74, 22))
		self.cb_fractureType.setObjectName("cb_fractureType")
		self.cb_fractureType.addItem("")
		self.cb_fractureType.addItem("")
		self.cb_fractureType.addItem("")
		self.cb_fractureType.setItemText(0, QtGui.QApplication.translate("Uniform", "Uniform", None, QtGui.QApplication.UnicodeUTF8))
		self.cb_fractureType.setItemText(1, QtGui.QApplication.translate("Cluster", "Cluster", None, QtGui.QApplication.UnicodeUTF8))
		self.cb_fractureType.setItemText(2, QtGui.QApplication.translate("Bezier", "Bezier", None, QtGui.QApplication.UnicodeUTF8))

		# split options
		self.split_options = QtGui.QFrame(self.tp_settings)
		self.split_options.setGeometry(QtCore.QRect(70, 40, 301, 16))
		self.split_options.setFrameShape(QtGui.QFrame.HLine)
		self.split_options.setFrameShadow(QtGui.QFrame.Sunken)
		self.split_options.setObjectName("split_options")

		# options label	
		self.lbl_options = QtGui.QLabel(self.tp_settings)
		self.lbl_options.setGeometry(QtCore.QRect(10, 35, 55, 19))
		self.lbl_options.setFont(font_12)
		self.lbl_options.setObjectName("lbl_options")
		self.lbl_options.setText(QtGui.QApplication.translate("Options", "Options", None, QtGui.QApplication.UnicodeUTF8))

		# uniform label
		self.lbl_uniform = QtGui.QLabel(self.tp_settings)
		self.lbl_uniform.setGeometry(QtCore.QRect(40, 65, 50, 16))
		self.lbl_uniform.setFont(font_10)
		self.lbl_uniform.setObjectName("lbl_uniform")
		self.lbl_uniform.setText(QtGui.QApplication.translate("Uniform", "Uniform", None, QtGui.QApplication.UnicodeUTF8))

		# spin uniform
		self.spin_uniform = QtGui.QSpinBox(self.tp_settings)
		self.spin_uniform.setGeometry(QtCore.QRect(90, 62, 81, 22))
		self.spin_uniform.setMaximum(999)
		self.spin_uniform.setObjectName("spin_uniform")

		# primary label
		self.lbl_primary = QtGui.QLabel(self.tp_settings)
		self.lbl_primary.setGeometry(QtCore.QRect(40, 93, 49, 16))
		self.lbl_primary.setFont(font_10)
		self.lbl_primary.setObjectName("lbl_primary")
		self.lbl_primary.setText(QtGui.QApplication.translate("Primary", "Primary", None, QtGui.QApplication.UnicodeUTF8))

		# spin primary
		self.spin_primary = QtGui.QSpinBox(self.tp_settings)
		self.spin_primary.setGeometry(QtCore.QRect(90, 90, 81, 22))
		self.spin_primary.setMaximum(999)
		self.spin_primary.setObjectName("spin_primary")

		# secondary label
		self.lbl_secondary = QtGui.QLabel(self.tp_settings)
		self.lbl_secondary.setGeometry(QtCore.QRect(25, 123, 65, 16))
		self.lbl_secondary.setFont(font_10)
		self.lbl_secondary.setObjectName("lbl_secondary")
		self.lbl_secondary.setText(QtGui.QApplication.translate("Secondary", "Secondary", None, QtGui.QApplication.UnicodeUTF8))

		# spin secondary
		self.spin_secondary = QtGui.QSpinBox(self.tp_settings)
		self.spin_secondary.setGeometry(QtCore.QRect(90, 120, 81, 22))
		self.spin_secondary.setMaximum(999)
		self.spin_secondary.setObjectName("spin_secondary")

		# flux label
		self.lbl_flux = QtGui.QLabel(self.tp_settings)
		self.lbl_flux.setGeometry(QtCore.QRect(212, 65, 23, 16))
		self.lbl_flux.setFont(font_10)
		self.lbl_flux.setObjectName("lbl_flux")
		self.lbl_flux.setText(QtGui.QApplication.translate("Flux", "Flux", None, QtGui.QApplication.UnicodeUTF8))

		# spin flux
		self.spin_flux = QtGui.QDoubleSpinBox(self.tp_settings)
		self.spin_flux.setGeometry(QtCore.QRect(237, 62, 81, 22))
		self.spin_flux.setObjectName("spin_flux")
		self.spin_flux.setMaximum(100.0)
		self.spin_flux.setSingleStep(0.1)

		# gap label
		self.lbl_gap = QtGui.QLabel(self.tp_settings)
		self.lbl_gap.setGeometry(QtCore.QRect(210, 93, 27, 16))
		self.lbl_gap.setFont(font_10)
		self.lbl_gap.setObjectName("lbl_gap")
		self.lbl_gap.setText(QtGui.QApplication.translate("Gap", "Gap", None, QtGui.QApplication.UnicodeUTF8))

		# spin gap
		self.spin_gap = QtGui.QDoubleSpinBox(self.tp_settings)
		self.spin_gap.setGeometry(QtCore.QRect(237, 90, 81, 22))
		self.spin_gap.setObjectName("spin_gap")
		self.spin_gap.setMaximum(100.0)
		self.spin_gap.setSingleStep(0.1)

		# seed label
		self.lbl_seed = QtGui.QLabel(self.tp_settings)
		self.lbl_seed.setGeometry(QtCore.QRect(185, 12, 31, 16))
		self.lbl_seed.setFont(font_10)
		self.lbl_seed.setObjectName("lbl_seed")
		self.lbl_seed.setText(QtGui.QApplication.translate("Seed", "Seed", None, QtGui.QApplication.UnicodeUTF8))

		# spin seed
		self.spin_seed = QtGui.QSpinBox(self.tp_settings)
		self.spin_seed.setGeometry(QtCore.QRect(220, 10, 81, 22))
		self.spin_seed.setObjectName("spin_speed")
		self.spin_seed.setMinimum(0)
		self.spin_seed.setMaximum(99999)

		# samples label
		self.lbl_samples = QtGui.QLabel(self.tp_settings)
		self.lbl_samples.setGeometry(QtCore.QRect(185, 120, 81, 22))
		self.lbl_samples.setFont(font_10)
		self.lbl_samples.setObjectName("lbl_samples")
		self.lbl_samples.setText(QtGui.QApplication.translate("Samples", "Samples", None, QtGui.QApplication.UnicodeUTF8))

		# spin samples
		self.spin_samples = QtGui.QSpinBox(self.tp_settings)
		self.spin_samples.setGeometry(QtCore.QRect(237, 120, 81, 22))
		self.spin_samples.setObjectName("spin_samples")
		self.spin_samples.setMaximum(999)

		# fracture split
		self.split_fracture = QtGui.QFrame(self.tp_settings)
		self.split_fracture.setGeometry(QtCore.QRect(10, 150, 355, 20))
		self.split_fracture.setFrameShape(QtGui.QFrame.HLine)
		self.split_fracture.setFrameShadow(QtGui.QFrame.Sunken)
		self.split_fracture.setObjectName("split_fracture")

		# fracture button
		self.btn_fracture = QtGui.QPushButton(self.tp_settings)
		self.btn_fracture.setGeometry(QtCore.QRect(10, 168, 355, 31))
		self.btn_fracture.setFont(font_12)
		self.btn_fracture.setObjectName("btn_fracture")
		self.btn_fracture.setText(QtGui.QApplication.translate("Fracture", "Fracture", None, QtGui.QApplication.UnicodeUTF8))
		self.btn_fracture.clicked.connect(self.__fracture_cb)

		# positions tab page
		self.tp_positions = QtGui.QWidget()
		self.tp_positions.setObjectName("tp_positions")
		self.ta_main.addTab(self.tp_positions, "")
		self.ta_main.setTabText(self.ta_main.indexOf(self.tp_positions), QtGui.QApplication.translate("Positions", "Positions", None, QtGui.QApplication.UnicodeUTF8))

		# positions list view
		self.lv_positions = QtGui.QListWidget(self.tp_positions)
		self.lv_positions.setGeometry(QtCore.QRect(10, 10, 355, 161))
		self.lv_positions.setObjectName("lv_positions")

		# move up button
		self.btn_move_up = QtGui.QPushButton(self.tp_positions)
		self.btn_move_up.setGeometry(QtCore.QRect(340, 10, 25, 20))
		self.btn_move_up.setObjectName("btn_move_up")
		self.btn_move_up.setText(QtGui.QApplication.translate("^", "^", None, QtGui.QApplication.UnicodeUTF8))
		self.btn_move_up.clicked.connect(self.__move_position_up)

		# move down button
		self.btn_move_down = QtGui.QPushButton(self.tp_positions)
		self.btn_move_down.setGeometry(QtCore.QRect(340, 35, 25, 20))
		self.btn_move_down.setObjectName("self.btn_move_down")
		self.btn_move_down.setText(QtGui.QApplication.translate("v", "v", None, QtGui.QApplication.UnicodeUTF8))
		self.btn_move_down.clicked.connect(self.__move_position_down)

		# clear button
		self.btn_clear = QtGui.QPushButton(self.tp_positions)
		self.btn_clear.setGeometry(QtCore.QRect(324, 177, 41, 23))
		self.btn_clear.setObjectName("btn_clear")
		self.btn_clear.setText(QtGui.QApplication.translate("Clear", "Clear", None, QtGui.QApplication.UnicodeUTF8))
		self.btn_clear.clicked.connect(self.__clear_cb)

		# remove button
		self.btn_remove = QtGui.QPushButton(self.tp_positions)
		self.btn_remove.setGeometry(QtCore.QRect(260, 177, 61, 23))
		self.btn_remove.setObjectName("btn_remove")
		self.btn_remove.setText(QtGui.QApplication.translate("Remove", "Remove", None, QtGui.QApplication.UnicodeUTF8))
		self.btn_remove.clicked.connect(self.__remove_cb)

		# add selected button
		self.btn_addSelected = QtGui.QPushButton(self.tp_positions)
		self.btn_addSelected.setGeometry(QtCore.QRect(10, 177, 81, 23))
		self.btn_addSelected.setObjectName("btn_addSelected")
		self.btn_addSelected.setText(QtGui.QApplication.translate("Add Selected", "Add Selected", None, QtGui.QApplication.UnicodeUTF8))
		self.btn_addSelected.clicked.connect(self.__add_selected_cb)

		# advanced tab page
		self.tp_advanced = QtGui.QWidget()
		self.tp_advanced.setObjectName("tp_advanced")
		self.ta_main.addTab(self.tp_advanced, "")
		self.ta_main.setTabText(self.ta_main.indexOf(self.tp_advanced), QtGui.QApplication.translate("Advanced", "Advanced", None, QtGui.QApplication.UnicodeUTF8))

		# multi-threaded checkbox
		self.chk_multithreaded = QtGui.QCheckBox(self.tp_advanced)
		self.chk_multithreaded.setGeometry(QtCore.QRect(10, 10, 150, 16))
		self.chk_multithreaded.setFont(font_10)
		self.chk_multithreaded.setObjectName("chk_multithreaded")
		self.chk_multithreaded.setText(QtGui.QApplication.translate("Use multiple threads", "Use multiple threads", None, QtGui.QApplication.UnicodeUTF8))
		self.chk_multithreaded.setChecked(True)

		# smoothing angle label
		self.lbl_smoothing = QtGui.QLabel(self.tp_advanced)
		self.lbl_smoothing.setGeometry(QtCore.QRect(10, 37, 100, 16))
		self.lbl_smoothing.setFont(font_10)
		self.lbl_smoothing.setObjectName("lbl_smoothing")
		self.lbl_smoothing.setText(QtGui.QApplication.translate("Smoothing Angle", "Smoothing Angle", None, QtGui.QApplication.UnicodeUTF8))

		# smoothing angle spin
		self.spin_smoothing = QtGui.QDoubleSpinBox(self.tp_advanced)
		self.spin_smoothing.setGeometry(QtCore.QRect(115, 35, 81, 22))
		self.spin_smoothing.setObjectName("spin_smoothing")
		self.spin_smoothing.setSingleStep(0.1)
		self.spin_smoothing.setMaximum(360.0)
		self.spin_smoothing.setMinimum(-360.0)
		self.spin_smoothing.setValue(30.0)

		# slicer label
		self.lbl_slicer = QtGui.QLabel(self.tp_advanced)
		self.lbl_slicer.setGeometry(QtCore.QRect(10, 100, 100, 16))
		self.lbl_slicer.setFont(font_10)
		self.lbl_slicer.setObjectName("lbl_slicer")
		self.lbl_slicer.setText(QtGui.QApplication.translate("Slicer", "Slicer", None, QtGui.QApplication.UnicodeUTF8))

		# slicer cb
		self.cb_slicer = QtGui.QComboBox(self.tp_advanced)
		self.cb_slicer.setGeometry(QtCore.QRect(45, 98, 74, 22))
		self.cb_slicer.setObjectName("cb_slicer")
		self.cb_slicer.addItem("")
		self.cb_slicer.addItem("")
		self.cb_slicer.setItemText(0, QtGui.QApplication.translate("GTE", "GTE", None, QtGui.QApplication.UnicodeUTF8))
		self.cb_slicer.setItemText(1, QtGui.QApplication.translate("CSGJS", "CSGJS", None, QtGui.QApplication.UnicodeUTF8))

		# min bezier distance label
		self.lbl_min_bezier_dist = QtGui.QLabel(self.tp_advanced);
		self.lbl_min_bezier_dist.setGeometry(QtCore.QRect(10, 67, 150, 16))
		self.lbl_min_bezier_dist.setFont(font_10)
		self.lbl_min_bezier_dist.setObjectName("lbl_min_bezier_dist")
		self.lbl_min_bezier_dist.setText(QtGui.QApplication.translate("Min Bezier Distance", "Min Bezier Distance", None, QtGui.QApplication.UnicodeUTF8))

		# min bezier distance spin
		self.spin_min_bezier_dist = QtGui.QDoubleSpinBox(self.tp_advanced)
		self.spin_min_bezier_dist.setGeometry(QtCore.QRect(130, 65, 81, 22))
		self.spin_min_bezier_dist.setObjectName("spin_min_bezier_dist")
		self.spin_min_bezier_dist.setSingleStep(0.1)
		self.spin_min_bezier_dist.setMinimum(0.0)
		self.spin_min_bezier_dist.setMaximum(100.0)
		self.spin_min_bezier_dist.setValue(50.0)

		# generated command label
		self.lbl_generatedCommand = QtGui.QLabel(self.tp_advanced)
		self.lbl_generatedCommand.setGeometry(QtCore.QRect(10, 160, 326, 16))
		self.lbl_generatedCommand.setFont(font_10)
		self.lbl_generatedCommand.setObjectName("lbl_generatedCommand")
		self.lbl_generatedCommand.setText(QtGui.QApplication.translate("Generated Command (updated on fracture)", "Generated Command (updated on fracture)", None, QtGui.QApplication.UnicodeUTF8))

		# generated command textbox
		self.txt_generatedCommand = QtGui.QLineEdit(self.tp_advanced)
		self.txt_generatedCommand.setGeometry(QtCore.QRect(10, 180, 355, 20))
		self.txt_generatedCommand.setReadOnly(True)
		self.txt_generatedCommand.setPlaceholderText("")
		self.txt_generatedCommand.setObjectName("txt_generatedCommand")
		
		# website label
		self.lbl_website = QtGui.QLabel(self)
		self.lbl_website.setGeometry(QtCore.QRect(300, 290, 87, 13))
		self.lbl_website.setObjectName("lbl_website")
		self.lbl_website.setText("<a href=\"www.ngreen.org\"><font color=white>www.ngreen.org</font></a>")
		self.lbl_website.setOpenExternalLinks(True)
		self.lbl_website.setTextFormat(QtCore.Qt.RichText)
		self.lbl_website.setTextInteractionFlags(QtCore.Qt.TextBrowserInteraction)

		# QtCore.QMetaObject.connectSlotsByName(self)
		self.show()
		self.raise_()
	#end

#end

def main():
	hadan_gui = HadanGui()
	return hadan_gui

if __name__ == '__main__':
	main()
