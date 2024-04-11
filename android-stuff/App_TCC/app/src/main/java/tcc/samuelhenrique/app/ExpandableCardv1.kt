package tcc.samuelhenrique.app

import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.core.animateFloatAsState
import androidx.compose.foundation.clickable
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.KeyboardArrowDown
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.rotate
import androidx.compose.ui.text.font.Font
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import tcc.samuelhenrique.app.ui.theme.Erro_Vermelho
import tcc.samuelhenrique.app.ui.theme.Texto

@Composable
fun ExpandableCardv1(titleCard: String,
                   description: @Composable () -> Unit,
                   gravity: @Composable () -> Unit,
                   currentReading: @Composable () -> Unit) {
    var expanded by remember { mutableStateOf(false) }
    val rotateState by animateFloatAsState(targetValue = if (expanded) 180f else 0f, label = "")

    Card (
        colors = CardDefaults.cardColors(
            containerColor = Erro_Vermelho,
            contentColor = Texto
        ),
        shape = RoundedCornerShape(16.dp),
        elevation = CardDefaults.cardElevation(
            defaultElevation = 16.dp
        ),
        modifier = Modifier
//            .clip(RoundedCornerShape(16.dp)) // O efeito de ripple respeita o shape do card
//            .clickable(onClick = { expanded = !expanded })
            .clickable(
                interactionSource = remember { MutableInteractionSource() },
                indication = null
            ) { expanded = !expanded } // Remove o efeito de click do card
            .fillMaxWidth()
//            .shadow(5.dp, shape = RoundedCornerShape(16.dp), spotColor = Texto),
//            .background(Background_Card)
//            .animateContentSize(animationSpec = tween(300)) //Vídeo 2
//            .animateContentSize( //Vídeo 3
//                animationSpec = tween(
//                    durationMillis = 300,
//                    easing = LinearEasing
//                )
//            ),

    ) {
        Column (
            modifier = Modifier
                .fillMaxWidth()
                .padding(12.dp)
        ){
            Row (verticalAlignment = Alignment.CenterVertically){
               Text(
                   modifier = Modifier.weight(6f),
                   text = titleCard,
                   fontSize = 32.sp,
                   fontFamily = FontFamily(
                       Font(R.font.abhayalibre_extrabold)
                   ),
                   color = Texto,
                   maxLines = 1,
                   overflow = TextOverflow.Ellipsis
               )
                IconButton(
                    onClick = {
                        expanded = !expanded
                    },
                    modifier = Modifier.rotate(rotateState)
//                        .alpha(0.5f)
//                    modifier = Modifier.weight(1f)
                ) {
                    Icon(imageVector = Icons.Default.KeyboardArrowDown,
                        contentDescription = "Drop-Down Arrow",
                        tint = Texto
                    )
                }
            }
//            if(expanded) {
            AnimatedVisibility(visible = expanded) {
                Column {
                    description()
                    gravity()
                    currentReading()
                }
            }
        }
    }
}

@Composable
fun Description1 (content: String) {
    Row {
        Text(
            text = "Descrição: ",
            fontSize = 16.sp,
            fontFamily = FontFamily(
                Font(R.font.abhayalibre_extrabold)
            ),
            color = Texto
        )

        Text(
            text = content,
            fontSize = 16.sp,
            fontFamily = FontFamily(
                Font(R.font.abhayalibre_regular)
            ),
            color = Texto
        )
    }
}

@Composable
fun Gravity1 (content: String) {
    Row {
        Text(
            text = "Gravidade: ",
            fontSize = 16.sp,
            fontFamily = FontFamily(
                Font(R.font.abhayalibre_extrabold)
            ),
            color = Texto
        )

        Text(text = content,
            fontSize = 16.sp,
            fontFamily = FontFamily(
                Font(R.font.abhayalibre_regular)
            ),
            color = Texto
        )
    }
}

@Composable
fun CurrentReading1 (content: String) {
    Row {
        Text(
            text = "Leitura atual: ",
            fontSize = 16.sp,
            fontFamily = FontFamily(
                Font(R.font.abhayalibre_extrabold)
            ),
            color = Texto
        )

        Text(
            text = content,
            fontSize = 16.sp,
            fontFamily = FontFamily(
                Font(R.font.abhayalibre_regular)
            ),
            color = Texto
        )
    }
}

@Preview(showBackground = true)
@Composable
fun ExandableCardPreview() {
    ExpandableCard(titleCard = "TMP-01",
        description = { Description1("Erro ao tentar conectar com o servidor") },
        gravity = { Gravity1("Média") },
        currentReading = { CurrentReading1("404") })
}