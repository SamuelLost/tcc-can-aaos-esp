package tcc.samuelhenrique.app.utils

import android.content.Context
import coil.Coil
import coil.ImageLoader
import coil.decode.SvgDecoder
import coil.disk.DiskCache
import coil.memory.MemoryCache
import coil.request.ImageRequest
import coil.size.Size

object ImageLoaderUtility {
    fun initImageLoader(context: Context) {
        val imageLoader = ImageLoader.Builder(context)
            .components {
                add(SvgDecoder.Factory())
            }
            .diskCache {
                DiskCache.Builder()
                    .maxSizeBytes(512L * 1024 * 1024) // 512MB
                    .build()
            }
//            .memoryCache {
//                MemoryCache.Builder(context)
//                    .maxSizeBytes(256 * 1024 * 1024) // 256MB
//                    .build()
//            }
            .build()

        Coil.setImageLoader(imageLoader)
    }

    fun preloadImages(context: Context, svgImages: List<String>) {
        val imageLoader = Coil.imageLoader(context)
        svgImages.forEach { imageUrl ->
            val request = ImageRequest.Builder(context)
                .data(imageUrl)
                .size(Size.ORIGINAL)
                .build()
            imageLoader.enqueue(request)
        }
    }
}
