# coding=utf-8

import sys
from PySide import QtCore, QtGui
import maya.cmds as cmds
import maya.mel as mel

class HadanGui(QtGui.QMainWindow):
	def __init__(self):
		super(HadanGui, self).__init__()
		self.__build_ui()
		self.__source_positions = {}
		self.__command_str = ''
	#end

	def __run_command(self):
		self.__recompute_command()
		mel.eval(self.__command_str)
	#end

	def __recompute_command(self):
		self.__command_str = 'hadan '
		self.__command_str += '-mn pCube1 '

		fracture_type = self.__get_current_fracture_type()

		if fracture_type == 'Uniform':
			self.__command_str += '-ft uniform '
			self.__command_str += '-uc %d ' % (self.spin_uniform_uniform.value())
		elif fracture_type == 'Cluster':
			self.__command_str += '-ft cluster '
			self.__command_str += '-uc %d ' % (self.spin_cluster_uniform.value())
			self.__command_str += '-pc %d ' % (self.spin_cluster_primary.value())
			self.__command_str += '-sc %d ' % (self.spin_cluster_secondary.value())
			self.__command_str += '-flp %f ' % (self.spin_cluster_flux.value())
		elif fracture_type == 'Bezier':
			self.__command_str += '-ft bezier '
			self.__command_str += '-uc %d ' % (self.spin_uniform_bezier.value())
			self.__command_str += '-flp %f ' % (self.spin_bezier_flux.value())

		for pnt in self.__source_positions:
			if not cmds.objExists(pnt):
				continue
			pos = cmds.xform(pnt, q=True, ws=True, t=True)
			self.__command_str += '-pnt %f %f %f ' % (pos[0], pos[1], pos[2])

		self.txt_generated_command.setText(self.__command_str)
	#end

	def __get_current_fracture_type(self):
		return self.cb_fracture_type.currentText()
	#end

	def __rebuild_source_positions_list(self):
		self.lv_source_positions.clear()
		for curr in self.__source_positions:
			self.lv_source_positions.addItem(curr)
	#end

	def __add_selected_callback(self):
		for curr in cmds.ls(sl=True, type='transform'):
			if curr in self.__source_positions:
				continue
			self.__source_positions[curr] = curr
		self.__rebuild_source_positions_list()
	#end

	def __remove_selected_callback(self):
		item = self.lv_source_positions.currentItem()
		if None is item:
			return
		del self.__source_positions[item.text()]
		self.__rebuild_source_positions_list()
	#end

	def __fracture_type_changed(self):
		new_mode = self.cb_fracture_type.currentText()
		if new_mode == 'Uniform':
			self.frame_uniform.setVisible(True)
			self.frame_cluster.setVisible(False)
			self.frame_bezier.setVisible(False)
		elif new_mode == 'Cluster':
			self.frame_uniform.setVisible(False)
			self.frame_cluster.setVisible(True)
			self.frame_bezier.setVisible(False)
		elif new_mode == 'Bezier':
			self.frame_uniform.setVisible(False)
			self.frame_cluster.setVisible(False)
			self.frame_bezier.setVisible(True)
	#end

	def __build_ui(self):
		# create shared fonts
		font_10 = QtGui.QFont()
		font_10.setPointSize(10)
		font_8 = QtGui.QFont()
		font_8.setPointSize(8)

		# window
		self.setObjectName("self")
		self.resize(448, 512)
		self.setWindowTitle(QtGui.QApplication.translate("Hadan", "破断", None, QtGui.QApplication.UnicodeUTF8))

		# lbl_title
		self.lbl_title = QtGui.QLabel(self)
		self.lbl_title.setGeometry(QtCore.QRect(170, 0, 112, 68))
		font = QtGui.QFont()
		font.setPointSize(42)
		self.lbl_title.setFont(font)
		self.lbl_title.setObjectName("lbl_title")
		self.lbl_title.setText(QtGui.QApplication.translate("Hadan", "破断", None, QtGui.QApplication.UnicodeUTF8))

		# tab_main
		self.tab_main = QtGui.QTabWidget(self)
		self.tab_main.setGeometry(QtCore.QRect(10, 70, 431, 371))
		self.tab_main.setObjectName("tab_main")
		self.tab_main.setCurrentIndex(0)

		# tabpage_basic
		self.tabpage_basic = QtGui.QWidget()
		self.tabpage_basic.setObjectName("tabpage_basic")

		# lbl_fracture_type
		self.lbl_fracture_type = QtGui.QLabel(self.tabpage_basic)
		self.lbl_fracture_type.setGeometry(QtCore.QRect(10, 8.5, 91, 21))
		self.lbl_fracture_type.setFont(font_10)
		self.lbl_fracture_type.setObjectName("lbl_fracture_type")
		self.lbl_fracture_type.setText("Fracture Type")

		# cb_fracture_type
		self.cb_fracture_type = QtGui.QComboBox(self.tabpage_basic)
		self.cb_fracture_type.setGeometry(QtCore.QRect(100, 10, 81, 20))
		self.cb_fracture_type.setFont(font_8)
		self.cb_fracture_type.setObjectName("cb_fracture_type")
		self.cb_fracture_type.addItem("Uniform")
		self.cb_fracture_type.addItem("Cluster")
		self.cb_fracture_type.addItem("Bezier")
		self.cb_fracture_type.currentIndexChanged.connect(self.__fracture_type_changed)

		# splitter_a
		self.splitter_a = QtGui.QFrame(self.tabpage_basic)
		self.splitter_a.setGeometry(QtCore.QRect(10, 30, 401, 20))
		self.splitter_a.setFrameShape(QtGui.QFrame.HLine)
		self.splitter_a.setFrameShadow(QtGui.QFrame.Sunken)
		self.splitter_a.setObjectName("splitter_a")

		# lbl_gap
		self.lbl_gap = QtGui.QLabel(self.tabpage_basic)
		self.lbl_gap.setGeometry(QtCore.QRect(10, 110, 61, 21))
		self.lbl_gap.setFont(font_10)
		self.lbl_gap.setObjectName("lbl_gap")
		self.lbl_gap.setText("Gap")

		# spinner_gap
		self.spinner_gap = QtGui.QDoubleSpinBox(self.tabpage_basic)
		self.spinner_gap.setGeometry(QtCore.QRect(10, 130, 62, 22))
		self.spinner_gap.setFont(font_8)
		self.spinner_gap.setObjectName("spinner_gap")

		# splitter_b
		self.splitter_b = QtGui.QFrame(self.tabpage_basic)
		self.splitter_b.setGeometry(QtCore.QRect(10, 100, 401, 20))
		self.splitter_b.setFrameShape(QtGui.QFrame.HLine)
		self.splitter_b.setFrameShadow(QtGui.QFrame.Sunken)
		self.splitter_b.setObjectName("splitter_b")

		# splitter_c
		self.splitter_c = QtGui.QFrame(self.tabpage_basic)
		self.splitter_c.setGeometry(QtCore.QRect(10, 160, 401, 20))
		self.splitter_c.setFrameShape(QtGui.QFrame.HLine)
		self.splitter_c.setFrameShadow(QtGui.QFrame.Sunken)
		self.splitter_c.setObjectName("splitter_c")

		# lbl_source_positions
		self.lbl_source_positions = QtGui.QLabel(self.tabpage_basic)
		self.lbl_source_positions.setGeometry(QtCore.QRect(10, 180, 101, 21))
		self.lbl_source_positions.setFont(font_10)
		self.lbl_source_positions.setObjectName("lbl_source_positions")
		self.lbl_source_positions.setText("Source Positions")

		# btn_remove_source_position
		self.btn_remove_source_position = QtGui.QPushButton(self.tabpage_basic)
		self.btn_remove_source_position.setGeometry(QtCore.QRect(350, 310, 61, 23))
		self.btn_remove_source_position.setFont(font_8)
		self.btn_remove_source_position.setObjectName("btn_remove_source_position")
		self.btn_remove_source_position.setText("Remove")
		self.btn_remove_source_position.clicked.connect(self.__remove_selected_callback)

		# btn_add_selected_source_position
		self.btn_add_selected_source_position = QtGui.QPushButton(self.tabpage_basic)
		self.btn_add_selected_source_position.setGeometry(QtCore.QRect(10, 310, 81, 23))
		self.btn_add_selected_source_position.setFont(font_8)
		self.btn_add_selected_source_position.setObjectName("btn_add_selected_source_position")
		self.btn_add_selected_source_position.setText("Add Selected")
		self.btn_add_selected_source_position.clicked.connect(self.__add_selected_callback)

		# lv_source_positions
		self.lv_source_positions = QtGui.QListWidget(self.tabpage_basic)
		self.lv_source_positions.setGeometry(QtCore.QRect(10, 200, 401, 101))
		self.lv_source_positions.setObjectName("lv_source_positions")

		# frame_uniform
		self.frame_uniform = QtGui.QFrame(self.tabpage_basic)
		self.frame_uniform.setGeometry(QtCore.QRect(10, 50, 410, 51))
		self.frame_uniform.setFrameShape(QtGui.QFrame.StyledPanel)
		self.frame_uniform.setFrameShadow(QtGui.QFrame.Raised)
		self.frame_uniform.setObjectName("frame_uniform")

		# spin_uniform_uniform
		self.spin_uniform_uniform = QtGui.QSpinBox(self.frame_uniform)
		self.spin_uniform_uniform.setGeometry(QtCore.QRect(10, 20, 101, 22))
		self.spin_uniform_uniform.setFont(font_10)
		self.spin_uniform_uniform.setObjectName("spin_uniform_uniform")

		# lbl_uniform_uniform
		self.lbl_uniform_uniform = QtGui.QLabel(self.frame_uniform)
		self.lbl_uniform_uniform.setGeometry(QtCore.QRect(10, 0, 101, 21))
		self.lbl_uniform_uniform.setFont(font_10)
		self.lbl_uniform_uniform.setObjectName("lbl_uniform_uniform")
		self.lbl_uniform_uniform.setText("Uniform")

		# add basic tab page
		self.tab_main.addTab(self.tabpage_basic, "")

		# tabpage_advanced
		self.tabpage_advanced = QtGui.QWidget()
		self.tabpage_advanced.setObjectName("tabpage_advanced")

		# txt_generated_command
		self.txt_generated_command = QtGui.QLineEdit(self.tabpage_advanced)
		self.txt_generated_command.setGeometry(QtCore.QRect(10, 30, 401, 20))
		self.txt_generated_command.setFont(font_8)
		self.txt_generated_command.setObjectName("txt_generated_command")

		# lbl_generated_command
		self.lbl_generated_command = QtGui.QLabel(self.tabpage_advanced)
		self.lbl_generated_command.setGeometry(QtCore.QRect(10, 10, 131, 16))
		self.lbl_generated_command.setFont(font_10)
		self.lbl_generated_command.setObjectName("lbl_generated_command")
		self.lbl_generated_command.setText("Generated Command")

		# add advanced tab page
		self.tab_main.addTab(self.tabpage_advanced, "")

		# frame_cluster
		self.frame_cluster = QtGui.QFrame(self.tabpage_basic)
		self.frame_cluster.setGeometry(QtCore.QRect(10, 50, 410, 51))
		self.frame_cluster.setFrameShape(QtGui.QFrame.StyledPanel)
		self.frame_cluster.setFrameShadow(QtGui.QFrame.Raised)
		self.frame_cluster.setObjectName("frame_cluster")

		# spin_cluster_uniform
		self.spin_cluster_uniform = QtGui.QSpinBox(self.frame_cluster)
		self.spin_cluster_uniform.setGeometry(QtCore.QRect(10, 20, 101, 22))
		self.spin_cluster_uniform.setFont(font_10)
		self.spin_cluster_uniform.setObjectName("spin_cluster_uniform")

		# lbl_cluster_uniform
		self.lbl_cluster_uniform = QtGui.QLabel(self.frame_cluster)
		self.lbl_cluster_uniform.setGeometry(QtCore.QRect(10, 0, 101, 21))
		self.lbl_cluster_uniform.setFont(font_10)
		self.lbl_cluster_uniform.setObjectName("lbl_cluster_uniform")
		self.lbl_cluster_uniform.setText("Uniform")

		# spin_cluster_flux
		self.spin_cluster_flux = QtGui.QDoubleSpinBox(self.frame_cluster)
		self.spin_cluster_flux.setGeometry(QtCore.QRect(340, 20, 62, 22))
		self.spin_cluster_flux.setFont(font_10)
		self.spin_cluster_flux.setObjectName("spin_cluster_flux")
		self.lbl_cluster_flux = QtGui.QLabel(self.frame_cluster)

		# lbl_cluster_flux
		self.lbl_cluster_flux.setGeometry(QtCore.QRect(340, 0, 61, 21))
		self.lbl_cluster_flux.setFont(font_10)
		self.lbl_cluster_flux.setObjectName("lbl_cluster_flux")
		self.lbl_cluster_flux.setText("Flux")

		# lbl_cluster_primary
		self.lbl_cluster_primary = QtGui.QLabel(self.frame_cluster)
		self.lbl_cluster_primary.setGeometry(QtCore.QRect(120, 0, 101, 21))
		self.lbl_cluster_primary.setFont(font_10)
		self.lbl_cluster_primary.setObjectName("lbl_cluster_primary")
		self.lbl_cluster_primary.setText("Primary")

		# spin_cluster_primary
		self.spin_cluster_primary = QtGui.QSpinBox(self.frame_cluster)
		self.spin_cluster_primary.setGeometry(QtCore.QRect(120, 20, 101, 22))
		self.spin_cluster_primary.setFont(font_10)
		self.spin_cluster_primary.setObjectName("spin_cluster_primary")

		# lbl_cluster_secondary
		self.lbl_cluster_secondary = QtGui.QLabel(self.frame_cluster)
		self.lbl_cluster_secondary.setGeometry(QtCore.QRect(230, 0, 101, 21))
		self.lbl_cluster_secondary.setFont(font_10)
		self.lbl_cluster_secondary.setObjectName("lbl_cluster_secondary")
		self.lbl_cluster_secondary.setText("Secondary")

		# spin_cluster_secondary
		self.spin_cluster_secondary = QtGui.QSpinBox(self.frame_cluster)
		self.spin_cluster_secondary.setGeometry(QtCore.QRect(230, 20, 101, 22))
		self.spin_cluster_secondary.setFont(font_10)
		self.spin_cluster_secondary.setObjectName("spin_cluster_secondary")

		# frame_bezier
		self.frame_bezier = QtGui.QFrame(self.tabpage_basic)
		self.frame_bezier.setGeometry(QtCore.QRect(10, 50, 410, 51))
		self.frame_bezier.setFrameShape(QtGui.QFrame.StyledPanel)
		self.frame_bezier.setFrameShadow(QtGui.QFrame.Raised)
		self.frame_bezier.setObjectName("frame_bezier")

		# spin_uniform_bezier
		self.spin_uniform_bezier = QtGui.QSpinBox(self.frame_bezier)
		self.spin_uniform_bezier.setGeometry(QtCore.QRect(10, 20, 101, 22))
		self.spin_uniform_bezier.setFont(font_10)
		self.spin_uniform_bezier.setObjectName("spin_uniform_bezier")

		# lbl_bezier_uniform
		self.lbl_bezier_uniform = QtGui.QLabel(self.frame_bezier)
		self.lbl_bezier_uniform.setGeometry(QtCore.QRect(10, 0, 101, 21))
		self.lbl_bezier_uniform.setFont(font_10)
		self.lbl_bezier_uniform.setObjectName("lbl_bezier_uniform")
		self.lbl_bezier_uniform.setText("Uniform")

		# spin_bezier_flux
		self.spin_bezier_flux = QtGui.QDoubleSpinBox(self.frame_bezier)
		self.spin_bezier_flux.setGeometry(QtCore.QRect(120, 20, 62, 22))
		self.spin_bezier_flux.setFont(font_10)
		self.spin_bezier_flux.setObjectName("spin_bezier_flux")

		# lbl_bezier_flux
		self.lbl_bezier_flux = QtGui.QLabel(self.frame_bezier)
		self.lbl_bezier_flux.setGeometry(QtCore.QRect(120, 0, 61, 21))
		self.lbl_bezier_flux.setFont(font_10)
		self.lbl_bezier_flux.setObjectName("lbl_bezier_flux")
		self.lbl_bezier_flux.setText("Flux")

		# tab text
		self.tab_main.setTabText(self.tab_main.indexOf(self.tabpage_basic), "Basic")
		self.tab_main.setTabText(self.tab_main.indexOf(self.tabpage_advanced), "Advanced")

		# set visibility of frames
		self.frame_uniform.setVisible(True)
		self.frame_cluster.setVisible(False)
		self.frame_bezier.setVisible(False)

		# btn_fracture
		self.btn_fracture = QtGui.QPushButton(self)
		self.btn_fracture.setGeometry(QtCore.QRect(10, 450, 431, 31))
		font = QtGui.QFont()
		font.setPointSize(16)
		self.btn_fracture.setFont(font)
		self.btn_fracture.setObjectName("btn_fracture")
		self.btn_fracture.setText("Fracture")
		self.btn_fracture.clicked.connect(self.__run_command)

		# lbl_copyright
		self.lbl_copyright = QtGui.QLabel(self)
		self.lbl_copyright.setGeometry(QtCore.QRect(340, 490, 101, 16))
		self.lbl_copyright.setFont(font_10)
		self.lbl_copyright.setObjectName("lbl_copyright")
		self.lbl_copyright.setText("www.ngreen.org")

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
