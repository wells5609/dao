<?php 

namespace Dao\Image\Adapter {

	/**
	 * Dao\Image\Adapter\GD
	 *
	 * Image manipulation support. Allows images to be resized, cropped, etc.
	 *
	 *<code>
	 *	$image = new Dao\Image\Adapter\GD("upload/test.jpg");
	 *	$image->resize(200, 200)->rotate(90)->crop(100, 100);
	 *	if ($image->save()) {
	 *		echo 'success';
	 *	}
	 *</code>
	 */
	
	class GD extends \Dao\Image\Adapter implements \Dao\Image\AdapterInterface {

		/**
		 * Checks if GD is enabled
		 *
		 * @return  boolean
		 */
		public static function check(){ }


		/**
		 * \Dao\Image\GD constructor
		 *
		 * @param string $file
		 */
		public function __construct($file, $width=null, $height=null){ }


		/**
		 * Execute a resize.
		 *
		 * @param int $width
		 * @param int $height
		 */
		protected function _resize($width, $height){ }


		/**
		 * This method scales the images using liquid rescaling method. Only support Imagick
		 *
		 * @param int $width   new width
		 * @param int $height  new height
		 * @param int $delta_x How much the seam can traverse on x-axis. Passing 0 causes the seams to be straight.
		 * @param int $rigidity Introduces a bias for non-straight seams. This parameter is typically 0.
		 * @return \Dao\Image\Adapter
		 */
		protected function _liquidRescale($width, $height, $delta_x, $regidity){ }


		/**
		 * Execute a crop.
		 *
		 * @param int $width
		 * @param int $height
		 * @param int $offset_x
		 * @param int $offset_y
		 */
		protected function _crop($width, $height, $offset_x, $offset_y){ }


		/**
		 * Execute a rotation.
		 *
		 * @param int $degrees
		 */
		protected function _rotate($degrees){ }


		/**
		 * Execute a flip.
		 *
		 * @param int $direction
		 */
		protected function _flip($direction){ }


		/**
		 * Execute a sharpen.
		 *
		 * @param int $amount
		 */
		protected function _sharpen($amount){ }


		/**
		 * Execute a reflection.
		 *
		 * @param int $height
		 * @param int $opacity
		 * @param boolean $fade_in
		 */
		protected function _reflection($height, $opacity, $fade_in){ }


		/**
		 * Execute a watermarking.
		 *
		 * @param \Dao\Image\Adapter $watermark
		 * @param int $offset_x
		 * @param int $offset_y
		 * @param int $opacity
		 */
		protected function _watermark($watermark, $offset_x, $offset_y, $opacity){ }


		/**
		 * Execute a text
		 *
		 * @param string text
		 * @param int $offset_x
		 * @param int $offset_y
		 * @param int $opacity
		 * @param int $r
		 * @param int $g
		 * @param int $b
		 * @param int $size
		 * @param string $fontfile
		 */
		protected function _text($text, $offset_x, $offset_y, $opacity, $r, $g, $b, $size, $fontfile){ }


		/**
		 * Composite one image onto another
		
		 *
		 * @param \Dao\Image\Adapter $mask  mask Image instance
		 */
		protected function _mask($mask){ }


		/**
		 * Execute a background.
		 *
		 * @param int $r
		 * @param int $g
		 * @param int $b
		 * @param int $opacity
		 */
		protected function _background($r, $g, $b, $opacity){ }


		/**
		 * Blur image
		 *
		 * @param int $radius Blur radius
		 */
		protected function _blur($radius){ }


		/**
		 * Pixelate image
		 *
		 * @param int $amount amount to pixelate
		 */
		protected function _pixelate($amount){ }


		/**
		 * Execute a save.
		 *
		 * @param string $file
		 * @param int $quality
		 * @return boolean
		 */
		protected function _save($file, $opacity=null, $interlacing=null){ }


		/**
		 * Execute a render.
		 *
		 * @param string $type
		 * @param int $quality
		 * @return string
		 */
		protected function _render($type, $opacity=null, $interlacing=null){ }


		/**
		 * Create an empty image with the given width and height.
		 *
		 * @param int $width
		 * @param int $height
		 * @return resource
		 */
		protected function _create($width, $height){ }


		/**
		 * Destroys the loaded image to free up resources.
		 */
		public function __destruct(){ }


		/**
		 * Draws a line
		 *
		 * @param int $sx
		 * @param int $sy
		 * @param int $ex
		 * @param int $ey
		 * @param string $color
		 * @return \Dao\Image\Adapter\GD
		 */
		public function line($sx, $sy, $ex, $ey, $color=null){ }


		/**
		 * Draws a polygon
		 *
		 *<code>
		 * $coordinates = array( array( 'x' => 4, 'y' => 6 ), array( 'x' => 8, 'y' => 10 ) );
		 * $image->polygon($coordinates);
		 *</code>
		 *
		 * @param array $coordinates array of x and y
		 * @param string $color
		 * @return \Dao\Image\Adapter\GD
		 */
		public function polygon($coordinates, $color=null){ }

	}
}
