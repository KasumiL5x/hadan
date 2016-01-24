# coding=utf-8

import sys
from PySide import QtCore, QtGui
import maya.cmds as cmds
import maya.mel as mel
import os

class HadanGui(QtGui.QMainWindow):
	def __init__(self):
		super(HadanGui, self).__init__()
		self.__build_ui()
	#end

	def __fracture_cb(self):
		print 'Not yet implemented.'
	#end

	def __add_selected_cb(self):
		print 'Not yet implemented.'
	#end

	def __remove_cb(self):
		print 'Not yet implemented.'
	#end

	def __clear_cb(self):
		print 'Not yet implemented.'
	#end

	def __how_to_use_cb(self):
		print 'Not yet implemented.'
	#end

	def __build_ui(self):
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
		font = QtGui.QFont()
		font.setPointSize(10)
		self.lbl_fractureType.setFont(font)
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
		font = QtGui.QFont()
		font.setPointSize(12)
		self.lbl_options.setFont(font)
		self.lbl_options.setObjectName("lbl_options")
		self.lbl_options.setText(QtGui.QApplication.translate("Options", "Options", None, QtGui.QApplication.UnicodeUTF8))

		# uniform label
		self.lbl_uniform = QtGui.QLabel(self.tp_settings)
		self.lbl_uniform.setGeometry(QtCore.QRect(40, 65, 50, 16))
		font = QtGui.QFont()
		font.setPointSize(10)
		self.lbl_uniform.setFont(font)
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
		font = QtGui.QFont()
		font.setPointSize(10)
		self.lbl_primary.setFont(font)
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
		font = QtGui.QFont()
		font.setPointSize(10)
		self.lbl_secondary.setFont(font)
		self.lbl_secondary.setObjectName("lbl_secondary")
		self.lbl_secondary.setText(QtGui.QApplication.translate("Secondary", "Secondary", None, QtGui.QApplication.UnicodeUTF8))

		# spin secondary
		self.spin_secondary = QtGui.QSpinBox(self.tp_settings)
		self.spin_secondary.setGeometry(QtCore.QRect(90, 120, 81, 22))
		self.spin_secondary.setMaximum(999)
		self.spin_secondary.setObjectName("spin_secondary")

		# flux label
		self.lbl_flux = QtGui.QLabel(self.tp_settings)
		self.lbl_flux.setGeometry(QtCore.QRect(192, 65, 23, 16))
		font = QtGui.QFont()
		font.setPointSize(10)
		self.lbl_flux.setFont(font)
		self.lbl_flux.setObjectName("lbl_flux")
		self.lbl_flux.setText(QtGui.QApplication.translate("Flux", "Flux", None, QtGui.QApplication.UnicodeUTF8))

		# spin flux
		self.spin_flux = QtGui.QDoubleSpinBox(self.tp_settings)
		self.spin_flux.setGeometry(QtCore.QRect(220, 62, 81, 22))
		self.spin_flux.setObjectName("spin_flux")

		# gap label
		self.lbl_gap = QtGui.QLabel(self.tp_settings)
		self.lbl_gap.setGeometry(QtCore.QRect(193, 93, 27, 16))
		font = QtGui.QFont()
		font.setPointSize(10)
		self.lbl_gap.setFont(font)
		self.lbl_gap.setObjectName("lbl_gap")
		self.lbl_gap.setText(QtGui.QApplication.translate("Gap", "Gap", None, QtGui.QApplication.UnicodeUTF8))

		# spin gap
		self.spin_gap = QtGui.QDoubleSpinBox(self.tp_settings)
		self.spin_gap.setGeometry(QtCore.QRect(220, 90, 81, 22))
		self.spin_gap.setObjectName("spin_gap")

		# fracture split
		self.split_fracture = QtGui.QFrame(self.tp_settings)
		self.split_fracture.setGeometry(QtCore.QRect(10, 150, 355, 20))
		self.split_fracture.setFrameShape(QtGui.QFrame.HLine)
		self.split_fracture.setFrameShadow(QtGui.QFrame.Sunken)
		self.split_fracture.setObjectName("split_fracture")

		# fracture button
		self.btn_fracture = QtGui.QPushButton(self.tp_settings)
		self.btn_fracture.setGeometry(QtCore.QRect(10, 168, 355, 31))
		font = QtGui.QFont()
		font.setPointSize(12)
		self.btn_fracture.setFont(font)
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

		# generated command label
		self.lbl_generatedCommand = QtGui.QLabel(self.tp_advanced)
		self.lbl_generatedCommand.setGeometry(QtCore.QRect(10, 10, 126, 16))
		font = QtGui.QFont()
		font.setPointSize(10)
		self.lbl_generatedCommand.setFont(font)
		self.lbl_generatedCommand.setObjectName("lbl_generatedCommand")
		self.lbl_generatedCommand.setText(QtGui.QApplication.translate("Generated Command", "Generated Command", None, QtGui.QApplication.UnicodeUTF8))

		# generated command textbox
		self.txt_generatedCommand = QtGui.QLineEdit(self.tp_advanced)
		self.txt_generatedCommand.setGeometry(QtCore.QRect(10, 30, 355, 20))
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
	app = QtGui.QApplication.instance()
	if not app:
		app = QtGui.QApplication(sys.argv)
		app.aboutToQuit.connect(app.deleteLater)
	global hadan_gui
	hadan_gui = HadanGui()
	sys.exit(app.exec_())
#end

if __name__ == '__main__':
	main()
#end
